#pragma once

#include <string>

namespace pg::err {
	// Used when a file doesn't exist
	struct FileNotExists {
    std::string file_name;
    std::string str() const {
      return std::string("The file \"") + file_name + "\" does not exist";
    }
  };
  // Used when a file cannotbe opened.
  struct FileNotOpened {
		std::string file_path;
		std::string str() const {
			return std::string("File \"") + file_path + "\" could not be opened.";
		}
	};

}
