#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <iostream>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <string>

# define DEFAULT_CONF	"./conf/default.conf"
# define NONE			0
# define EXACT			1
# define PREFERENTIAL	2
# define ON				1
# define OFF			0

int							printErr (std::string errMsg);
std::vector<std::string>	split (std::string str, char delimiter = '\n');
std::vector<std::string>	splitServerBlocks (std::string block);
std::vector<std::string>	splitLocationBlocks (std::string block);
std::pair<bool, size_t>		skipKey (std::string line, std::string str);
bool						isNumber (std::string str);
std::string					parseValue (std::string line, size_t pos);
int							strToInt (std::string str);


#include <sys/types.h> //st_mtime
#include <sys/event.h> //kevent
#include <sys/time.h> //timeval
#include <sys/socket.h>
#include <sys/stat.h> // struct stat
#include <arpa/inet.h> //sockaddr_in
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h> //DIR
#include <set>

#include <limits> //numeric_limits

#include <cstring> //memset

# define CGI_BUF_SIZE 65536
# define ERROR_HTML "<!DOCTYPE html>\n\
	<html lang=\"en\">\n\
	\t<meta charset=\"utf-8\">\n\
	\t<title>40404</title>\n\
	<body>\n\
	\t<h1>NO ERROR PAGE</h1>\n\
	\t<p>configuration file has no error page that fits error code</p>\n\
	</body>\n\
	</html>\n"

enum	ErrorCode
{
	Continue = 100, //지금까지 상태가 괜찮다는 임시적인 응답
	OK = 200, //요청이 성공적으로 됨
	Created = 201, //요청이 성공하여 새로운 리소스 생성
	No_Content = 204, //요청에 대해서 보내줄 콘텐츠가 없다

	Bad_Request = 400, //요청이 잘못된 문법으로 이루어져 있을 때
	Forbidden = 403, //서버가 클라이언트가 누군지 알고 있고, 클라이언트가 권리가 없을 때
	Not_Found = 404, //요청받은 리소스를 찾지 못했을 때
	Method_Not_Allowed = 405, //요청한 메소드를 서버에서 알고는 있지만 금지했을 때
	//GET과 HEAD는 필수 메소드이기 때문에 제거하면 안된다.
	Payload_Too_Large = 413, //요청 엔티티가 서버에서 정의한 한계보다 클 때
	//서버는 연결을 끊거나 Retry-After헤더필드로 돌려 보냄
	Internal_Server_error = 500, //서버가 처리 방법을 모를 때

	//사용할지 말지 모르는 것들
	Moved_Permanently = 301, //요청한 리소스의 URI가 변경되었음을 의미
	Found = 302, //요청한 리소스의 URI가 일시적으로 변경되었음을 의미
	See_Other = 303, //요청한 리소스를 다른 URI에서 GET요청을 통해 얻어야 할 때
	Unauthorized = 401, //서버가 클라이언트가 누군지 모르고, 클라이언트가 권리가 없을 때
	Request_Timeout = 408, //요청을 한지 시간이 오래된 연결에 전송
	Conflict = 409, //요청이 현재 서버의 상태와 충돌할 때
	Length_Required = 411, //요청이 Content-Length헤더 필드가 정의되지 않을 때
	Precondition_Failed = 412, //클라이언트의 헤더에 있는 전제조건이 서버의 전제조건에 적절하지 않을 때
	URI_Too_Long = 414, //클라이언트가 요청한 URI가 서버에서 처리하지 않기로 한 길이보다 길 때
	Unsupported_Media_Type = 415, //요청한 미디어 포맷을 서버에서 지원하지 않을 때
	Requested_Range_Not_Satisfiable = 416, //Range 헤더 필드에 요청한 지정 범위를 만족시킬 수 없을 때
	Too_Many_Requests = 429, //user가 주어진 시간 내에서 너무 많은 request를 보냈을 때
	Request_Header_Fields_Too_Large = 431, //요청한 헤더 필드가 너무 클 때
	Bad_Gateway = 502, //서버가 요청을 처리하는 데 필요한 응답을 얻기 위해 게이트웨이로 작업하는 동안 잘못된 응답을 수신했을 때
	Service_Unavailable = 503, //서버가 요청을 처리할 준비가 되지 않았을 때
	HTTP_Version_Not_Supported = 505 //요청에 사용된 HTTP 버전을 서버에서 지원하지 않을 때
};

enum	BodyExist
{
	No_Body = 0,
	Body_Exist = 1,
	Body_Start = 2,
	Body_End = 3
};

typedef struct s_listen
{
	unsigned int	host;
	int				port;
}	t_listen;


unsigned int	host_to_int(std::string host);
//string형인 host를 사용할 수 있도록 unsigned int형으로 바꿔준다.

int	pathIsFile(const std::string& path);
//파일이 REG(regular file)이면 1을 리턴하고 다른 경우에는 0을 리턴한다.

std::string	readHtml(const std::string& path, std::string* type);
//path가 REGULAR file이면 open하여 파일 내용을 리턴한다.
//open이 실패하면 error_html을 리턴하고, open 성공시 type을 text/html로 바꾸고 파일 내용 리턴
//path가 REGULAR file이 아니면 error_html을 리턴한다.

//autoindex가 켜져 있을 때 string형으로 html문법을 구성한다.
std::string	set_uri(const std::string& dirList, const std::string& dirName,
	const std::string& host, const int port);
std::string	set_html(const std::string& path, const std::string& lang,
	const std::string& charset, const std::string& h1, const std::string& host, const int port);
	//path:파일의 경로, lang:어떤 언어를 사용할지 ex) en, charset: 문자 인코딩 방식 ex) utf-8
	//h1: 제목

//config server.cpp의 parseErrPages에 집어넣으면 될 듯?
//string형인 errPages를 받아와서, errMap의 key값에 error_code, value값에 html을 넣어준다.
int	set_error_page(const std::string& errPages, std::map<int, std::string>* errMap);

std::string	intToStr(int code);

void	print_vec(std::vector<std::string> str_vec);

int	compare_end(const std::string& s1, const std::string& s2);

#endif
