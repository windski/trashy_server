//
// Created by falleuion on 8/26/17.
//

#include "script_parse.h"
#include <fstream>
#include <regex>

// TODO: 完成实现...
namespace config_parse {

	config_file_parse::config_file_parse()
	: static_path(new char[100]),
	  pages_path(new char[100]),
	  config_fileno(0),
	  page_file_type(nullptr)
	{
		std::fstream file(DEFAULT_CONFIG_FILE_PATH, std::ios::in);
		if(!file.is_open()) {
			logging(WARN, "Can NOT OPEN configure file.. from %s", DEFAULT_CONFIG_FILE_PATH);
		}

//		std::shared_ptr<char> tmp_file_buff(new char[net::MAXLINE]);
//		file.read(tmp_file_buff.get(), net::MAXLINE);
//		if(file.is_open()) {
//			logging(WARN, "configuration file is big...");
//		}
		std::string tmp_file_buff;

		while(!file.is_open()) {
			std::getline(file, tmp_file_buff);
			const char *comment_ch = nullptr;

			if(((comment_ch = strchr(tmp_file_buff.c_str(), '#')) != NULL)) {
				size_t head_to_comment_len = str_offset(tmp_file_buff.c_str(), comment_ch);
				std::string real_str = real_str.substr(0, head_to_comment_len);
				if(real_str.empty())
					continue;               // 整行都是注释. 不解析...
				tmp_file_buff.clear();
				tmp_file_buff = std::move(real_str);
			}

			const char *pointer_ch = strchr(tmp_file_buff.c_str(), ':');
			size_t left_len = str_offset(tmp_file_buff.c_str(), pointer_ch);
			std::string head_port = std::move(tmp_file_buff.substr(0, left_len));
			logging(DEBUG, "split the string that head is %s.", head_port.c_str());

			std::string tail_port = std::move(tmp_file_buff.substr(left_len+1, tmp_file_buff.length()));
			logging(DEBUG, "split the string that tail is %s.", tail_port.c_str());

			path_KV_data.emplace_back(std::make_pair(head_port, tail_port));
		}

	}

	config_file_parse::~config_file_parse()
	{

	}

	std::string config_file_parse::get_static_path(void)
	{
		parse_setting();
		return std::string(static_path.get());
	}

	std::string config_file_parse::get_pages_path(void)
	{
		parse_setting();
		return std::string(pages_path.get());
	}

	void config_file_parse::parse_setting(void)
	{
		for(auto index : path_KV_data) {
			// 这玩意跟http header 那儿的解析一样都用了if-else串...应该影响不了性能吧...   ╮（﹀＿﹀）╭
			// 是的, 丑的一匹...
			if(std::regex_search(index.first, std::regex("listen_port"))) {
				config_file_listen_port = atoi(index.second.c_str());

			} else if(std::regex_search(index.first, std::regex("location"))) {
				root_dir = std::move(index.second);

			} else if(std::regex_search(index.first, std::regex("html_pages"))) {
				std::string tmp(index.second);
				tmp.insert(0, root_dir);
				check_dir(tmp);
				strcpy(pages_path.get(), tmp.c_str());

			} else if(std::regex_search(index.first, std::regex("static_path"))) {
				std::string tmp(index.second);
				tmp.insert(0, root_dir);
				check_dir(tmp);
				strcpy(static_path.get(), tmp.c_str());
			}
		}
	}

	void config_file_parse::check_dir(std::string &str)
	{
		size_t i;
		for(i = 0; i < str.length(); i++) {
			if(str[i] == '/' && str[i+1] == '/') {
				std::string tmp_str_l = str.substr(0, (i));
				std::string tmp_str_r = str.substr((i + 1), str.length());
				tmp_str_l += tmp_str_r;
				str = std::move(tmp_str_l);
			}
		}

		i = str.length() - 1;
		if(str[i] == '/') {
			while(str[i--] != '/');

			std::string tmp = str.substr(0, i);
			str.clear();
			str = std::move(tmp);
		}

	}

}       // end of namespace rewrite_tools
