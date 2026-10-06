#pragma once

#include <string>

namespace pg::err {

  struct ModifyAssetsFolderAfterInit {
    std::string folder_name;
    std::string str() const {
      return std::string("Attempted to set the assets folder to \"") +
        folder_name +
        " after the main loop started. Please only set the folder within the init(...) function.";
    }
  };

}
