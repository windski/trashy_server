//
// Created by falleuion on 10/5/17.
// 作死玩下模板,heiheihei
// 可以,,越写越觉得不合适....
// 666

#ifndef TRASHY_SERVER_SEARCH_DIR_HXX
#define TRASHY_SERVER_SEARCH_DIR_HXX

//#include "../config.h"
#include <stack>
#include <dirent.h>
#include "../log.h"
#include <cstring>

namespace rewrite_tool
{

	namespace utils {

		bool file_found;
        typedef std::stack<DIR *> storage_t;
        storage_t storage_root;

	}  // end of utils


    void release_dir()
    {
        using namespace utils;
        while(!storage_root.empty()) {
            DIR * tmp = storage_root.top();
            closedir(tmp);
            storage_root.pop();
        }
    }


	template <typename T>
	class srch_t;

	template <>
	class srch_t<char *>
	{
	public:
		typedef char * srch_base_t;
		inline std::string init(srch_base_t tmp)
		{
			return std::string(tmp);
		}
	};

	template <>
	class srch_t<std::string>
	{
	public:
		typedef std::string & srch_base_t;
		inline std::string init(srch_base_t tmp)
		{
			return tmp;
		}
	};

	template <>
	class srch_t<const char *>
	{
	public:
		typedef const char * srch_base_t;
		inline std::string init(srch_base_t tmp)
		{
			return std::string(tmp);
		}
	};


    void list_them(std::string &path, std::string &target, bool check_all = true)
    {
        struct dirent *entry = nullptr;
        DIR * dp = opendir(path.c_str());
        if(dp == nullptr) {
            logging(ERROR, "%s open Failure  ==>  %s, %d", path.c_str(), __FILE__, __LINE__);
            throw std::runtime_error("Directory failure");
        }

        std::string current_dir = path;
        if(*current_dir.rbegin() != '/')
            current_dir.insert(current_dir.length(), "/");

        while((entry = readdir(dp))) {
            if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
                continue;
            }

            if(entry->d_type != DT_DIR) {
                if(check_all) {
                    std::string tmp(entry->d_name);
	                if(tmp == target) {
		                utils::file_found = true;
	                }
                } else {
                    std::string::size_type tmp_index;
                    std::string::size_type tgt_index;
                    std::string tmp(entry->d_name);

                    tmp_index = tmp.rfind(".");
                    tgt_index = target.rfind(".");

                    std::string sub_tmp = tmp.substr(0, tmp_index);
                    std::string sub_tgt = target.substr(0, tgt_index);

	                if(sub_tmp == sub_tgt) {
		                utils::file_found = true;
	                }
                }
            } else {
                std::string next_dir(entry->d_name);
                std::string tmp = current_dir + next_dir;

                if(check_all) {
                    utils::storage_root.push(dp);
                    list_them(tmp, target);
                } else {
                    utils::storage_root.push(dp);
                    list_them(tmp, target, false);
                }
            }
        }

    }

	template <typename T>
    inline bool search_file(T dir, T target)
    {
        srch_t<T> trait_dir;
        std::string dir_s = trait_dir.init(dir);
        std::string tg_name_s = trait_dir.init(target);

        utils::file_found = false;
        list_them(dir_s, tg_name_s);
        release_dir();

        return utils::file_found;
    }

    template <typename T>
    inline bool search_file_ignore_extension(T dir, T target)
    {
        srch_t<T> trait_dir;
        std::string dir_s = trait_dir.init(dir);
        std::string tg_s = trait_dir.init(target);

	    utils::file_found = false;
	    list_them(dir_s, tg_s, false);
        release_dir();

	    return utils::file_found;

    }

}     // end of rewrite_tool

#endif //TRASHY_SERVER_SEARCH_DIR_HXX
