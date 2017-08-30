//
// Created by falleuion on 8/26/17.
//

#ifndef TRASHY_SERVER_SCRIPT_PARSE_H
#define TRASHY_SERVER_SCRIPT_PARSE_H

#include "config.h"
#include <vector>
#include <memory>

// TODO: configuration file parse


#define str_offset(lfhd, rihd) ((size_t)((rihd) - (lfhd)))

namespace config_parse {

	class config_file_parse {
	private:
		typedef std::shared_ptr<char> route_path_t;
		route_path_t static_path;
		route_path_t pages_path;
		int config_file_listen_port;
		int config_fileno;
		std::string page_file_type;
		std::vector<std::pair<std::string, std::string>> path_KV_data;           // paths and values' data.
		std::string root_dir;
	private:
		inline void parse_setting(void);
		void check_dir(std::string &str);
	public:
		explicit config_file_parse();

		~config_file_parse();

		std::string get_static_path(void);

		std::string get_pages_path(void);
	};
}

#endif //TRASHY_SERVER_SCRIPT_PARSE_H
