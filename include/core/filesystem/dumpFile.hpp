#pragma once

/* Created by Gordie Novak on 7/31/26.
 * Purpose: 
 * Allows user to dump file into string instantly */

#include <filesystem>
#include <string>
#include <fstream>
#include <toolkit/filesystem/path.hpp>

namespace pg {

    inline std::string dumpFile(const path& path) {
        // 1. Open the file in binary mode (avoids line ending conversions)
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return "";

        // Gets the file size
        auto size = std::filesystem::file_size(path);

        std::string content(size, '\0');
        file.read(content.data(), size);

        return content;
    }
}
