#include "toolkit/filesystem/filesystem.hpp"

namespace pig {
    void files::init_filesystem() {
        internal_path_obj::working_directory = std::filesystem::current_path();
    }

    files::files() { init_filesystem(); }

    void files::initialize_filesystem() {
        internal_path_obj::working_directory = std::filesystem::current_path();
        internal_path_obj::working_directory = std::filesystem::absolute(internal_path_obj::working_directory).parent_path();
        set_assets_folder(internal_path_obj::working_directory.c_str());
    }

    void files::set_assets_folder(const char p[]) {
        internal_path_obj::assets_folder = internal_path_obj::working_directory / (std::filesystem::path(p));
        if (!std::filesystem::is_directory(internal_path_obj::assets_folder)) {
            std::filesystem::path newPath = internal_path_obj::working_directory.parent_path() / p;
            if (!std::filesystem::exists(newPath) && !std::filesystem::is_directory(internal_path_obj::assets_folder)) {
                err::panic("core::filesystem::set_assets_folder()", "Unable to locate assets_old folder ", internal_path_obj::assets_folder.c_str());
            } else {
                internal_path_obj::assets_folder = newPath;
            }
        }
    }

    path files::assets() {
        return path(internal_path_obj::assets_folder);
    }

    path files::working_dir() {
        return path(internal_path_obj::working_directory);
    }

    path dir(const char p[]) {
        const auto len = strlen(p);
        for (size_t i = 0; i < len; i++) {
            if (p[i] == '/' || p[i] == '\\') {
                err::panic("core::dir(const char path[])"
                    , "Cannot initialize a path with '\\' or '/'. Please use the notation:\n"
                       "\tpig::dir(\"directory\")/\"subdir\"/\"file.ext\"");
            }
        }
        return pig::path(std::filesystem::path(p));
    }
}
