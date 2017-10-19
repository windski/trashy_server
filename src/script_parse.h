//
// Created by falleuion on 8/26/17.
//

#ifndef TRASHY_SERVER_SCRIPT_PARSE_H
#define TRASHY_SERVER_SCRIPT_PARSE_H

#include "config.h"
#include "tool_util.h"


namespace config_parse {

	inline unsigned long str_offset_L(char *lfhd, const char *rihd)
	{
		if(rihd == nullptr || lfhd == nullptr)
			return 0;
		unsigned long i = 0;
		while(lfhd[i]) {
			if(lfhd[i] != *rihd)
				i++;
			else
				break;
		}
		return i;
	}

	class config_file_parse {
	private:
		std::string static_path;
		std::string pages_path;
		int config_file_listen_port;
		std::string page_file_type;
		std::vector<std::pair<std::string, std::string>> path_KV_data;           // paths and values' data.
		std::string root_dir;
		std::string check_sumV5;       // configuration file's MD5;

	private:
		inline void parse_setting(void);
		void check_dir(std::string &str);
		std::string calcul_file_md5();
		std::vector<std::pair<std::string, std::string>> load_file_to();
		void remove_space(std::string &str) const noexcept;

	public:
		explicit config_file_parse();

		~config_file_parse();

		std::string get_static_path(void);

		std::string get_pages_path(void);

		int get_listen_port(void);
	};

}

#endif //TRASHY_SERVER_SCRIPT_PARSE_H
