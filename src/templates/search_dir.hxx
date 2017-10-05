//
// Created by falleuion on 10/5/17.
// 作死玩下模板,heiheihei
// 可以,,越写越觉得不合适....
// 666

#ifndef TRASHY_SERVER_SEARCH_DIR_HXX
#define TRASHY_SERVER_SEARCH_DIR_HXX

#include "../config.h"
#include <stack>

namespace rewrite_tool
{
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

	template <typename T>
	void close_all_dir(T & stack_p)
	{
		for(auto i : stack_p) {
			closedir(i);
		}
	}

	template <typename T>
	inline int search_dir(T & tmp_dir_n, T & target_name)
	{
		typedef typename srch_t<T>::srch_base_t srch_base_t;

		srch_t<T> trait_dir_1;
		std::string dir_s = trait_dir_1.init(tmp_dir_n);
		std::string tg_name_s = trait_dir_1.init(target_name);

		DIR *dr = nullptr;
		if((dr = opendir(dir_s.c_str())) == nullptr) {
			logging(ERROR, "%s, %d, The dir '%s' can NOT open", __FILE__, __LINE__, dir_s.c_str());
			return -1;
		}

		struct dirent *drent = nullptr;

		std::stack<DIR *>stack_cp;

		again:         // first to use goto label...
		do {

			if(((drent = readdir(dr)) != nullptr) && (drent->d_type == DT_DIR)) {
				stack_cp.push(dr);

				if((dr = opendir(drent->d_name)) == nullptr) {
					logging(ERROR, "%s, %d, The dir '%s' can NOT open", __FILE__, __LINE__, dir_s.c_str());
					return -1;
				}
				continue;
			} else {
				if(strcmp((drent->d_name), tg_name_s.c_str()) == 0) {
					closedir(dr);
					close_all_dir(stack_cp);
					return 0;
				}

			}

		} while(drent == nullptr);

		if(!stack_cp.empty()) {
			dr = stack_cp.top();
			stack_cp.pop();
			goto again;
		}

		return 1;
	}



}     // end of rewrite_tool

#endif //TRASHY_SERVER_SEARCH_DIR_HXX
