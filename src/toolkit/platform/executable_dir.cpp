#include <toolkit/platform/executable_dir.hpp>
#include <filesystem>

// Platform-specific headers
#ifdef _WIN32
	#include <windows.h>
#elif defined(__linux__)
	#include <unistd.h>
	#include <limits.h>
#elif defined(__APPLE__)
	#include <mach-o/dyld.h>
#endif

namespace fs = std::filesystem;

std::string pg::platform::getExecutableDir() {
	[[maybe_unused]] constexpr uint64_t PATH_MAX_B = 1024;
#ifdef _WIN32
    wchar_t buffer[PATH_MAX_B];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return fs::path(buffer).parent_path().string();

#elif defined(__linux__)
    char buffer[PATH_MAX_B];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return fs::path(buffer).parent_path().string();
    }

#elif defined(__APPLE__)
    char buffer[PATH_MAX_B];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        return fs::canonical(fs::path(buffer)).parent_path().string();
    }
#endif
    return fs::current_path().string();
}
