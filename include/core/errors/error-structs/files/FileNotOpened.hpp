#pragma once

namespace pg::err {

	struct FileNotOpened {
		std::string file_path;
		std::string str() const {
			return std::string("File \"") + file_path + "\" could not be opened.";
		}
	};

}
