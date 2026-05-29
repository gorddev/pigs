#include "toolkit/filesystem/path.hpp"

namespace pg {
    std::filesystem::path internal_path_obj::working_directory = std::filesystem::path("@null");
    std::filesystem::path internal_path_obj::assets_folder = std::filesystem::path("..") / "assets";

    void path::verifyInputString(const char p[]) const {
        const auto len = strlen(p);
        for (size_t i = 0; i < len; i++) {
            if (p[i] == '/' || p[i] == '\\') {
                panic("path::operator/()",
                    "Cannot initialize path \"", (filepath / p).c_str(), "\" with '\\' or '/'. Please use the notation:\n"
                    "\tpg::dir(\"directory\")/\"subdir\"/\"file.ext\"");
            }
        }
    }

    path::path(std::filesystem::path filepath)
        : filepath(std::move(filepath)) {}

    path::path(const char p[])
        : filepath(std::filesystem::path(p)) {}

    path path::operator/(const char p[]) {
        verifyInputString(p);
        return path(filepath / p);
    }

    path::operator const char*() const {
        return filepath.c_str();
    }

    path& path::operator=(const char p[]) {
        verifyInputString(p);
        filepath = std::filesystem::path(p);
        return *this;
    }

    const char* path::c_str() const {
        return filepath.c_str();
    }

    std::filesystem::path path::std() const {
        return filepath;
    }

    path path::expand() const {
        return path(internal_path_obj::assets_folder / filepath);
    }

    bool path::is_regular_file() const {
        const path expanded = expand();
        if (!std::filesystem::exists(expanded.c_str())) {
            PIG_WriteLog("path::exists()",
                "The file ", expanded.c_str(), " doesn't exist.");
            return false;
        }
        if (!std::filesystem::is_regular_file(expanded.c_str())) {
            PIG_WriteLog("path::exits()",
                "The file ", expanded.c_str(), " is not a regular file.");
            return false;
        }
        return true;
    }

    path path::subdir(const char p[]) const {
        return pg::path(filepath / p);
    }
}
