//
// Created by windsky on 17-8-10.
//

#ifndef TRASHY_SERVER_TOOL_UTIL_H
#define TRASHY_SERVER_TOOL_UTIL_H

#include "config.h"
#include "script_parse.h"

// base of MD5 func.
#define F(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~(z))))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~(z))))

// Constants for MD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a, b, c, d, x, s, av) {               \
	(a) += F((b), (c), (d)) + (x) + (av);        \
	(a) = ROTATE_LEFT((a), (s));                 \
	(a) += (b);                                  \
}

#define GG(a, b, c, d, x, s, av) {               \
	(a) += G((b), (c), (d)) + (x) + (av);        \
	(a) = ROTATE_LEFT((a), (s));                 \
	(a) += (b);                                  \
}

#define HH(a, b, c, d, x, s, av) {               \
	(a) += H((b), (c), (d)) + (x) + (av);        \
	(a) = ROTATE_LEFT((a), (s));                 \
	(a) += (b);                                  \
}

#define II(a, b, c, d, x, s, av) {               \
	(a) += I((b), (c), (d)) + (x) + (av);        \
	(a) = ROTATE_LEFT((a), (s));                 \
	(a) += (b);                                  \
}


namespace rewrite_tool {

    class my_str : public std::string {
    public:
        explicit my_str(char *str);
        in_addr_t to_addr(net::IP_VERSION ipv) const;
    };

    extern void setnonblocking(int);

    class http_parse
    {
    private:
        int version;
        enum method { GET, PUT, DELETE, POST, HEAD };
        method request_method;
        std::shared_ptr<std::string> route;
        std::string host;
    private:
        int split_(std::vector<std::string>&, const std::string &, const std::string&& ) const;
//        inline int split_(std::pair<std::string, std::string> &, const std::string &, const char);
        void setting_attrib(std::string &source);
        void get_method(int);
	    void head_method(int);
	    inline bool __find_str_s(std::string&, const char ) const;
	    inline bool check_post_use_get() const;
    public:
        explicit http_parse(char *);
        explicit http_parse(std::string &);
        int make_response(int fileno);

        ~http_parse();
    };



	class MD5 {
	private:
		const size_t BUFF_LEN = 1024;
		typedef unsigned char byte_t;

		static const byte_t padding[64];
		static const char hex[16];

		byte_t __buff[64];
		byte_t _digest[16];
		ulong _magic_num[4];
		ulong _count[2];
		bool _finish;

	private:
		void update(const byte_t *in, size_t len);
		void transformers(const byte_t block[]);
		void decode(const byte_t *in, ulong *out, size_t len);
		void encode(const ulong *in, byte_t *out, size_t len);
		void final();
		std::string bytes_to_string(const byte_t *in, size_t len);
		MD5(const MD5&);
		MD5 &operator=(const MD5 &);

	public:
		explicit MD5();
		explicit MD5(std::string &str);
		~MD5();
		void reset();
		void update(std::fstream &in_file);
		void update(const std::string &str);
		void update(const void *in, size_t len);
		const byte_t *digest();
		std::string to_string();

	};


	class time_hp
	{
	private:
		inline std::string __get_time_stamp(std::string &&) const;
	public:
		time_hp();
		~time_hp();
		std::string get_time_stamp_c_style() const;
		std::string get_time_stamp_net_std1() const;
		std::string get_time_stamp_net_std2() const;
		std::string g_tm_fmt(const char *) const;
	};

} // end of rewrite_tool
#endif //TRASHY_SERVER_MY_STR_H
