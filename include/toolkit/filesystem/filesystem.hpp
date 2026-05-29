#pragma once

#include "path.hpp"

namespace pg {

    class Engine;

    /// Thin wrapper around the filesystem to allow for easy management
    /// of
    class files {
        /// Initializes the filesystem with the current path.
        static void init_filesystem();

        friend class Engine;


    public:
        /// constructor that initializes the filesystem
        files();

        static void initialize_filesystem();

        /// Sets the assets_old folder of the engine
        static void set_assets_folder(const char path[]);

        /// Gets the assets_old folder of the filesystem
        static path assets();

        /// Gets the working directory of the filesystem
        static path working_dir();

    };

    /// Gets a subdirectory of the assets_old folder
    path dir(const char path[]);

    inline files internal_filesystem_setup;

}
