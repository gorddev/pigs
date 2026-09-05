#pragma once

/* Created by Gordie Novak on 7/31/26.
 * Purpose:
 * Allows user to dump file into string instantly */


#include <filesystem>
#include <string>
#include <fstream>
#include <core/filesystem/path.hpp>

#include <errors/Error.hpp>
#include "errors/error-structs/files/FileNotExistsError.hpp"
#include "errors/error-structs/files/FileNotOpened.hpp"

namespace pg {

    inline expected<
    	std::string,
    	err::FileNotOpened,
     	err::FileNotExists>
    dumpFile(const path& path) {
        if (!std::filesystem::exists(path))
        	return PG_UErrNew(err::FileNotExists, .file_name = path.c_str());
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        	return PG_UErrNew(err::FileNotOpened, .file_path = path.c_str());

        // Gets the file size
        auto size = std::filesystem::file_size(path);

        std::string content(size, '\0');
        file.read(content.data(), size);

        return content;
    }
}
