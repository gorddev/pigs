#include <toolkit/platform/memory/memory_usage.hpp>

#include <fstream>
#include <filesystem>
#include <cstddef>
#include <algorithm>
#include <fstream>
#include <string>

#include "toolkit/platform/platform.hpp"


#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <psapi.h>

#elif defined(__APPLE__)
    #include <unistd.h>
    #include <climits>
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <mach/mach.h>
    #include <mach-o/dyld.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>

#elif defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #include <emscripten/heap.h>
#endif


// Returns the size of the current executable in bytes (0 on failure)
pg::mem::bytes pg::mem::get_executable_size() {
    if constexpr(platform::emscripten)
        return {0};

#if defined(_WIN32)
    else if constexpr(platform::win32) {
        wchar_t path[MAX_PATH];
        DWORD length = GetModuleFileNameW(NULL, path, MAX_PATH);
        if (length == 0 || length == MAX_PATH) return 0;
        return std::filesystem::file_size(path);
    }
#elif defined(__APPLE__)
    else if constexpr(platform::apple) {
        char path[PATH_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) != 0) return {0};
        return {std::filesystem::file_size(path)};
    }
#elif defined(__linux__)
    else if constexpr(platform::linux) {
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        if (count <= 0 || count >= PATH_MAX) return 0;
        path[count] = '\0';
        return std::filesystem::file_size(path);
    }
#endif
    return {0};
}


pg::mem::bytes pg::mem::get_max_allocation_size() {
    const size_t arch_limit = std::numeric_limits<std::size_t>::max();

    #if defined(_WIN32)
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status)) {
        std::size_t system_max = static_cast<std::size_t>(std::min(status.ullAvailVirtual, status.ullAvailPageFile));
        return std::min(arch_limit, system_max);
    }
    return arch_limit;

    #elif defined(__APPLE__)
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vm_stats;
    host_t host_port = mach_host_self();

    if (host_statistics(host_port, HOST_VM_INFO, reinterpret_cast<host_info_t>(&vm_stats), &count) == KERN_SUCCESS) {
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size > 0) {
            std::size_t available_ram = static_cast<std::size_t>(vm_stats.free_count) * static_cast<std::size_t>(page_size);
            return {std::min(arch_limit, available_ram)};
        }
    }
    // Fallback to total hardware memory if stats fail
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    int64_t total_memory = 0;
    size_t len = sizeof(total_memory);
    if (sysctl(mib, 2, &total_memory, &len, nullptr, 0) == 0) {
        return {std::min(arch_limit, static_cast<std::size_t>(total_memory))};
    }
    return {arch_limit};

    #elif defined(__linux__)
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGESIZE);
    if (pages > 0 && page_size > 0) {
        std::size_t available_ram = static_cast<std::size_t>(pages) * static_cast<std::size_t>(page_size);
        return std::min(arch_limit, available_ram);
    }
    return arch_limit;

    #elif defined(__EMSCRIPTEN__)
    return emscripten_get_heap_max() - emscripten_get_heap_size();

    #else
    return arch_limit;
    #endif
}

pg::mem::bytes pg::mem::get_process_memory_usage() {
    #if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<std::size_t>(pmc.WorkingSetSize);
    }
    return 0;

    #elif defined(__APPLE__)
    mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &infoCount) == KERN_SUCCESS) {
        return {static_cast<std::size_t>(info.resident_size)};
    }
    return {0};

    #elif defined(__linux__)
    std::ifstream statm("/proc/self/statm");
    if (statm.is_open()) {
        std::size_t size, resident;
        if (statm >> size >> resident) {
            long page_size = sysconf(_SC_PAGESIZE);
            if (page_size > 0) {
                return resident * static_cast<std::size_t>(page_size);
            }
        }
    }
    return 0;

    #elif defined(__EMSCRIPTEN__)
    return static_cast<std::size_t>(emscripten_get_heap_size());

    #else
    return 0;
    #endif
}


pg::mem::bytes pg::mem::get_system_memory_available() {
    #if defined(_WIN32)
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status)) {
        return static_cast<std::size_t>(status.ullAvailPhys);
    }
    return 0;

    #elif defined(__APPLE__)
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vm_stats;
    host_t host_port = mach_host_self();

    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stats, &count) == KERN_SUCCESS) {
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size > 0) {
            std::size_t available_pages = vm_stats.free_count + vm_stats.speculative_count;
            return {available_pages * static_cast<std::size_t>(page_size)};
        }
    }
    return {0};

    #elif defined(__linux__)
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string token;
        while (meminfo >> token) {
            if (token == "MemAvailable:") {
                std::size_t mem_kb;
                if (meminfo >> mem_kb) {
                    return mem_kb * 1024; // Convert KiB to Bytes
                }
            }
        }
    }
    return 0;

    #elif defined(__EMSCRIPTEN__)
    return static_cast<std::size_t>(emscripten_get_heap_max() - emscripten_get_heap_size());

    #else
    return 0;
    #endif
}
