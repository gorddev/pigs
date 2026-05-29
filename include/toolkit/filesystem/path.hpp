#pragma once

#include <filesystem>

#include <core/errors/pig_err.hpp>

namespace pg {

    class internal_path_obj {
        /// Working directory of the filesystem
        static std::filesystem::path working_directory;

        /// Folder assets_old are contained in.
        static std::filesystem::path assets_folder;

        friend class path;
        friend class files;
    };

    class path {
    private:
        /// The internal filesystem path this represents.
        std::filesystem::path filepath;

        friend class files; ///< Filesystem is a friend class

        void verifyInputString(const char path[]) const;

    public:
        explicit path(std::filesystem::path filepath);

        path(const char path[]);

        /// Add files onto the current path.
        path operator/(const char p[]);

        /// Automatic decay into a constant character for conversion purposes.
        // ReSharper disable once CppNonExplicitConversionOperator
        operator const char*() const; // NOLINT(*-explicit-constructor)

        path& operator=(const char path[]);

        /// Converts into a c_str
        [[nodiscard]] const char* c_str() const;

        /// Converts to a standard library filesystem object.
        [[nodiscard]] std::filesystem::path std() const;

        /// Expands the path into the form (assets_folder/THIS)
        [[nodiscard]] path expand() const;

        [[nodiscard]] bool is_regular_file() const;

        /// Adds a subdirectory to the current path
        path subdir(const char path[]) const;
    };
}
