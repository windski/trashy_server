//
// Created by falleuion on 8/26/17.
//

#include "script_parse.h"
#include "log.h"
#include <regex>
#include <fstream>

// TODO: 完成实现...
namespace config_parse {

	config_file_parse::config_file_parse()
	: static_path(new char (100)),
	  pages_path(new char (100)),
	  config_fileno(0)
	{
		std::fstream file(DEFAULT_CONFIG_FILE_PATH, std::ios::in);
		if(!file.is_open()) {
			logging(WARN, "Can NOT OPEN configure file.. from %s", DEFAULT_CONFIG_FILE_PATH);
		}
	}

	config_file_parse::~config_file_parse()
	{

	}

	std::string config_file_parse::get_static_path() const
	{

	}

	std::string config_file_parse::get_site_path() const
	{

	}

}       // end of namespace rewrite_tools
