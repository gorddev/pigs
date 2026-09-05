#pragma once

#include <string>
namespace pg::err {

  struct FileNotExists {
    std::string file_name;
    std::string str() const {
      return std::string("The file \"") + file_name + "\" does not exist";
    }
  };
}
