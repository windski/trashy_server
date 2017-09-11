//
// Created by windsky on 17-8-10.
//

#include "tool_util.h"
#include "Server.h"
#include <regex>


in_addr_t rewrite_tool::my_str::to_addr(net::IP_VERSION ipv) const
{
    char *tmp = nullptr;
    in_addr_t data = 0;
    tmp = const_cast<char *>(c_str());

    if(ipv == net::IPV4) {
        inet_pton(AF_INET, tmp, &data);
    } else if(ipv == net::IPV6) {
        inet_pton(AF_INET6, tmp, &data);
    }
    if(errno != EINVAL) {
        return data;
    }
}

rewrite_tool::my_str::my_str(char *str)
{

}


rewrite_tool::http_parse::http_parse(char * http_str)
{
    std::string tmp_str(http_str);
    std::vector<std::string> splited_str;
    split_(splited_str, tmp_str, "\r\n");       // 将字符串以\r\n分割
    std::vector<std::string> route_str;
    split_(route_str, splited_str[0], " ");

    route = std::make_shared<std::string>(route_str[1]);

	// 突然..发现, 可以在分割http request 后将其分段标号, 这样就不用在setting_attrib中用丑陋的if-else 串了... ˋ▽ˊ
    for(auto i : splited_str) {
        setting_attrib(i);
    }

}

rewrite_tool::http_parse::http_parse(std::string)
{

}

int rewrite_tool::http_parse::split_(std::vector<std::string>&result, const std::string &source, const std::string s)
{
    std::string::size_type rhs, lhs;
    lhs = 0;
    rhs = source.find(s);

    while(std::string::npos != rhs) {
        result.push_back(source.substr(lhs, (rhs - lhs)));

        lhs = rhs + s.size();
        rhs = source.find(s, lhs);
    }
//    if(lhs != source.length()) {
//        result.push_back(s.substr(lhs));
//    }

    return 0;
}

rewrite_tool::http_parse::~http_parse()
{

}

void rewrite_tool::http_parse::setting_attrib(std::string &source)
{
	// TODO: 用switch, 看上面 `突然..` 注释 (´・ω・`)
    if(std::regex_search(source, std::regex("(GET)"))) {
        request_method = GET;
    } else if(std::regex_search(source, std::regex("(POST)"))) {
        request_method = POST;
    } else if(std::regex_search(source, std::regex("(PUT)"))) {
        request_method = PUT;
    } else if(std::regex_search(source, std::regex("(DELETE)"))) {
        request_method = DELETE;
    } else if(std::regex_search(source, std::regex("(HEAD)"))) {
        request_method = HEAD;
    } else if(std::regex_search(source, std::regex("(HTTP/1.1)"))) {
        version = 1;
    } else if(std::regex_search(source, std::regex("(Host:)"))) {
        std::string::size_type index = source.find("Host");
        host = source.substr(index + 5);
    }
}

int rewrite_tool::http_parse::make_response(int fileno)
{
    switch(request_method) {
        case GET:
            get_method(fileno);
            break;

            // TODO POST methods etc. =_=

        default:
            break;
    }

}

void rewrite_tool::http_parse::get_method(int sockfd)
{
	config_parse::config_file_parse file_path;
	std::string tmp_path = file_path.get_pages_path();

	if(*tmp_path.rbegin() != '/')
		tmp_path.insert(tmp_path.length(), "/");

	if(*route == "/") {
		route->clear();
		(*route) = "index.html";
	} else {
		std::string &&tmp_route = route->substr(1, route->length());
		route->clear();
		(*route) = tmp_route;
	}

	route->insert(0, tmp_path);

    net::GET_Respose G_res(*route);
    G_res.response(sockfd);

    // TODO write a script to storage path.

    close(sockfd);
}



void rewrite_tool::setnonblocking(int fileno)
{
    int flags;

    if((flags = fcntl(fileno, F_GETFL)) < 0) {
        perror("fcntl get error");
        exit(EXIT_FAILURE);
    }

    flags |=  O_NONBLOCK;

    if(fcntl(fileno, F_SETFL, flags) < 0) {
        perror("fcntl set error");
        exit(EXIT_FAILURE);
    }
}



const rewrite_tool::MD5::byte_t rewrite_tool::MD5::padding[64] = { 0x80 };    // 一不留神, 这么长..  _(:з」∠)_
const char rewrite_tool::MD5::hex[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
};

rewrite_tool::MD5::MD5()
: _finish(false)
{
	reset();
}

rewrite_tool::MD5::MD5(std::string &str)
: _finish(false)
{
	reset();
	update(str);
}

rewrite_tool::MD5::~MD5()
{

}

void rewrite_tool::MD5::reset()
{
	_finish = false;
	_count[0] = _count[1] = 0;

	// magic number..
	_magic_num[0] = 0x67452301;
	_magic_num[1] = 0xefcdab89;
	_magic_num[2] = 0x98badcfe;
	_magic_num[3] = 0x10325476;
}

void rewrite_tool::MD5::update(const std::string &str)
{
	update(reinterpret_cast<const byte_t*>(str.c_str()), str.length());
}

void rewrite_tool::MD5::update(std::fstream &in_file)
{
	if(!in_file) {
		return ;
	}

	std::streamsize len;
	char buffer[BUFF_LEN];
	while(!in_file.eof()) {
		in_file.read(buffer, BUFF_LEN);
		len = in_file.gcount();
		if(len > 0) {
			update(buffer, len);
		}
	}

	in_file.close();
}

void rewrite_tool::MD5::update(const void *in, size_t len)
{
	update(static_cast<const byte_t *>(in), len);
}

void rewrite_tool::MD5::update(const rewrite_tool::MD5::byte_t *in, size_t len)
{
	ulong i, index, part_len;

	_finish = false;

	index = static_cast<ulong>((_count[0] >> 3) & 0x3f);

	if((_count[0] += static_cast<ulong>(len << 3)) < static_cast<ulong>(len << 3))
		_count[1]++;
	_count[1] += static_cast<ulong>(len >> 29);

	part_len = 64 - index;

	if(len >= part_len) {
		memcpy(&__buff[index], in, part_len);
		transformers(__buff);

		for(i = part_len; (i + 63) < len; i += 64) {
			transformers(&(in[i]));
		}

		index = 0;
	} else {
		i = 0;
	}
}

// 相信自己一段时间后用脚趾头都能想出这是跟什么用的...
void rewrite_tool::MD5::transformers(const rewrite_tool::MD5::byte_t *block)
{
	ulong a = _magic_num[0], b = _magic_num[1], c = _magic_num[2], d = _magic_num[3];
	ulong x[16];

	decode(block, x, 64);

	// Round 1 FIGHT!!
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756);
	FF (c, d, a, b, x[ 2], S13, 0x242070db);
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee);
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf);
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a);
	FF (c, d, a, b, x[ 6], S13, 0xa8304613);
	FF (b, c, d, a, x[ 7], S14, 0xfd469501);
	FF (a, b, c, d, x[ 8], S11, 0x698098d8);
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af);
	FF (c, d, a, b, x[10], S13, 0xffff5bb1);
	FF (b, c, d, a, x[11], S14, 0x895cd7be);
	FF (a, b, c, d, x[12], S11, 0x6b901122);
	FF (d, a, b, c, x[13], S12, 0xfd987193);
	FF (c, d, a, b, x[14], S13, 0xa679438e);
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	// Round 2 AGAIN
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340);
	GG (c, d, a, b, x[11], S23, 0x265e5a51);
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d);
	GG (d, a, b, c, x[10], S22,  0x2441453);
	GG (c, d, a, b, x[15], S23, 0xd8a1e681);
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6);
	GG (d, a, b, c, x[14], S22, 0xc33707d6);
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87);
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed);
	GG (a, b, c, d, x[13], S21, 0xa9e3e905);
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8);
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9);
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	// Round 3 They broken even!!!Extra time!!
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681);
	HH (c, d, a, b, x[11], S33, 0x6d9d6122);
	HH (b, c, d, a, x[14], S34, 0xfde5380c);
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44);
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9);
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60);
	HH (b, c, d, a, x[10], S34, 0xbebfbc70);
	HH (a, b, c, d, x[13], S31, 0x289b7ec6);
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa);
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085);
	HH (b, c, d, a, x[ 6], S34,  0x4881d05);
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039);
	HH (d, a, b, c, x[12], S32, 0xe6db99e5);
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8);
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	// Round 4 the WON is !!!
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97);
	II (c, d, a, b, x[14], S43, 0xab9423a7);
	II (b, c, d, a, x[ 5], S44, 0xfc93a039);
	II (a, b, c, d, x[12], S41, 0x655b59c3);
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92);
	II (c, d, a, b, x[10], S43, 0xffeff47d);
	II (b, c, d, a, x[ 1], S44, 0x85845dd1);
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f);
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0);
	II (c, d, a, b, x[ 6], S43, 0xa3014314);
	II (b, c, d, a, x[13], S44, 0x4e0811a1);
	II (a, b, c, d, x[ 4], S41, 0xf7537e82);
	II (d, a, b, c, x[11], S42, 0xbd3af235);
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
	// Nasty Johnny!!   ٩(๑´0`๑)۶

	_magic_num[0] += a;
	_magic_num[1] += b;
	_magic_num[2] += c;
	_magic_num[3] += d;
}

void rewrite_tool::MD5::decode(const rewrite_tool::MD5::byte_t *in, ulong *out, size_t len)
{
	for(size_t i = 0, j = 0; j < len; i++, j += 4) {
		out[i] = (static_cast<ulong>(in[j])) | (static_cast<ulong>(in[j + 1] << 8)) |
				(static_cast<ulong>(in[j + 2] << 16)) | (static_cast<ulong>(in[j + 3] << 24));
	}
}

void rewrite_tool::MD5::encode(const ulong *in, rewrite_tool::MD5::byte_t *out, size_t len)
{
	for(size_t i = 0, j = 0; j < len; i++, j+=4) {
		out[j] = static_cast<byte_t >(in[i] & 0xff);
		out[j + 1] = static_cast<byte_t >((in[i] >> 8) & 0xff);
		out[j + 2] = static_cast<byte_t >((in[i] >> 16) & 0xff);
		out[j + 3] = static_cast<byte_t >((in[i] >> 24) & 0xff);
	}
}

void rewrite_tool::MD5::final()
{
	byte_t bits[8];
	ulong old_magic_num[4];
	ulong old_count[2];
	ulong index, pad_len;

	// save current magic number
	memcpy(old_magic_num, _magic_num, sizeof(_magic_num));
	memcpy(old_count, _count, sizeof(_count));

	// save number of bits
	encode(_count, bits, 8);

	// pad out to 56 mod 64
	index = static_cast<ulong >((_count[0] >> 3) & 0x3f);
	pad_len = (index < 56) ? (56 - index) : (120 - index);
	update(padding, pad_len);

	// append length (before padding)
	update(bits, 8);

	// store state in digest
	encode(_magic_num, _digest, 16);

	memcpy(_magic_num, old_magic_num, sizeof(_magic_num));
	memcpy(_count, old_count, sizeof(_count));
}

const rewrite_tool::MD5::byte_t *rewrite_tool::MD5::digest()
{
	if(!_finish) {
		_finish = true;
		final();
	}
	return _digest;      // safe => non_const --> const
}

std::string rewrite_tool::MD5::bytes_to_string(const rewrite_tool::MD5::byte_t *in, size_t len)
{
	std::string str;
	str.reserve(len << 1);

	for(size_t i = 0; i < len; i++) {
		int t = in[i];
		int a = t / 16;
		int b = t % 16;
		str.append(1, hex[a]);
		str.append(1, hex[b]);
	}
	return str;
}

std::string rewrite_tool::MD5::to_string()
{
	return bytes_to_string(digest(), 16);
}

