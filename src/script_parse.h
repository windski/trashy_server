//
// Created by falleuion on 8/26/17.
//

#ifndef TRASHY_SERVER_SCRIPT_PARSE_H
#define TRASHY_SERVER_SCRIPT_PARSE_H

#include "config.h"

#include <memory>

// TODO: configuration file parse


namespace config_parse {

	class config_file_parse {
	private:
//	std::string static_path;
//	std::string pages_path;
		typedef std::shared_ptr<char> route_path_t;
		route_path_t static_path;
		route_path_t pages_path;
		int config_fileno;
		std::string page_file_type;
	public:
		explicit config_file_parse();

		~config_file_parse();

		std::string get_static_path(void) const;

		std::string get_site_path(void) const;
	};
}

#endif //TRASHY_SERVER_SCRIPT_PARSE_H
