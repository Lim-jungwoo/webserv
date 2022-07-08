#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

# include "EntityHeader.hpp"

class ResponseHeader : public EntityHeader
{
	public:
		ResponseHeader() { resetValues(); }
		ResponseHeader(const ResponseHeader& rh) { (void)rh; }
		virtual ~ResponseHeader() {}

		ResponseHeader&	operator=(const ResponseHeader& rh)
		{
			(void)rh;
			return (*this);
		}
	
		void	initErrorMap()
		{
			this->_error_map[Continue] = "Continue";
			this->_error_map[OK] = "OK";
			this->_error_map[Created] = "Created";
			this->_error_map[No_Content] = "No Content";
			this->_error_map[Bad_Request] = "Bad Request";
			this->_error_map[Forbidden] = "Forbidden";
			this->_error_map[Not_Found] = "Not Found";
			this->_error_map[Payload_Too_Large] = "Payload Too Large";
			this->_error_map[Internal_Server_error] = "Internal Server Error";
		}

		std::string	getHeader(size_t size, const std::string& path, int code, std::string type,
			const std::string& contentLocation, const std::string& lang)
		{
			std::string	header;
			resetValues();
			setValues(size, path, code, type, contentLocation, lang);
			header = "HTTP/1.1 " + intToStr(code) + " " + getStatusMessage(code) + "\r\n";
			header += writeHeader();
			return (header);
		}

		std::string	notAllowed(const std::set<std::string>& methods, const std::string& path,
			int code, const std::string& lang)
		{//이거 어따쓰는 거?
			std::string	header = "";
			resetValues();
			setValues(0, path, code, "", path, lang);
			setAllow(methods);
			if (code == Method_Not_Allowed)
				header += "HTTP/1.1 405 Method Not Allowed\r\n";
			else if (code == Payload_Too_Large)
				header += "HTTP/1.1 413 Payload Too Large\r\n";
			header += writeHeader();
			return (header);
		}

		std::string	writeHeader()
		{
			std::string	header = "";
			if (this->_allow != "")
				header += "Allow: " + this->_allow + "\r\n";
			
			//request header와 겹침
			if (this->_content_language != "")
				header += "Content-Language: " + this->_content_language + "\r\n";
			if (this->_content_length != "")
				header += "Content-Length: " + this->_content_length + "\r\n";
			if (this->_content_location != "")
				header += "Content-Location: " + this->_content_location + "\r\n";
			if (this->_content_type != "")
				header += "Content-Type: " + this->_content_type + "\r\n";
			//
			
			if (this->_date != "")
				header += "Date: " + this->_date + "\r\n";
			if (this->_last_modified != "")
				header += "Last-Modified: " + this->_last_modified + "\r\n";
			if (this->_location != "")
				header += "Location: " + this->_location + "\r\n";
			if (this->_retry_after != "")
				header += "Retry-After: " + this->_retry_after + "\r\n";
			if (this->_server != "")
				header += "Server: " + this->_server + "\r\n";

			//request header와 겹치는 것
			if (this->_transfer_encoding != "")
				header += "Transfer-Encoding: " + this->_transfer_encoding + "\r\n";
			//
			
			if (this->_www_authenticate != "")
				header += "WWW-Authenticate: " + this->_www_authenticate + "\r\n";

			return (header);
		}

		std::string	getStatusMessage(int code)
		{
			if (this->_error_map.find(code) != this->_error_map.end())
				return (this->_error_map[code]);
			return ("There is no error code");
		}

		void	resetValues()
		{
			this->_allow = "";
			this->_content_language = "";
			this->_content_length = "";
			this->_content_location = "";
			this->_content_type = "";
			this->_date = "";
			this->_last_modified = "";
			this->_location = "";
			this->_retry_after = "";
			this->_server = "";
			this->_transfer_encoding = "";
			this->_www_authenticate = "";
			this->initErrorMap();
		}

		void	setValues(size_t size, const std::string& path, int code, std::string type,
			const std::string& contentLocation, const std::string& lang)
		{
			//this->setAllow();
			this->setContentLanguage(lang);
			this->setContentLength(size);
			this->setContentLocation(contentLocation);
			this->setContentType(type, path);
			this->setDate();
			this->setLastModified(path);
			this->setLocation(code, path);
		}

		void	setServer()
		{ this->_server = "Webserv/1.0 (Unix)"; }
		void	setWwwAuthenticate(int code)
		{
			if (code == Unauthorized)
				this->_www_authenticate = "Basic realm=\"Access requires authentification\", charset=\"UTF-8\"";
		}
		void	setRetryAfter(int code, int sec)
		{
			if (code == Service_Unavailable || code == Too_Many_Requests || code == Moved_Permanently)
				this->_retry_after = intToStr(sec);
		}

		void	setTransferEncoding()
		{//transfer_encoding을 일단 압축이나 수정이 없는 버전으로 초기화
			this->_transfer_encoding = "identity";
		}

		void	setLastModified(const std::string& path)
		{
			char		buf[100];
			struct stat	file_stat;
			struct tm*	tm;
			if (stat(path.c_str(), &file_stat) == 0)
			{
				tm = gmtime(&file_stat.st_mtime);
				strftime(buf, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
				std::string	last_modified(buf);
				this->_last_modified = last_modified;
			}
		}

		void	setLocation(int code, const std::string& redirect)
		{
			if (code == Created || code / 100 == 3)
				this->_location = redirect;
		}
		
	private:
		std::string					_server; //웹서버 정보
		std::string					_www_authenticate; //사용자 인증이 필요한 자원을 요구할 시, 서버가 제공하는 인증 방식
		std::string					_retry_after; //다시 접속하라고 알릴 때
		std::string					_transfer_encoding; //사용자에게 entity를 안전하게 전송하기 위해 사용하는 인코딩 형식을 지정
		std::string					_last_modified; //리소스의 마지막 수정 날짜
		std::string					_location; //300번대 응답이나 201(created)응답일 때 어느 페이지로 이동할 지 알려주는 헤더
		
		std::map<int, std::string>	_error_map;

		//사용안할 것 같은 것
		std::string	_access_control_allow_origin; //요청 Host와 응답 Host가 다를 때 CORS에러를 막기 위해 사용
		std::string	_age; //시간이 얼마나 흘렀는지 초 단위로 알려줌
		std::string	_referrer_policy; //서버 referrer정책을 알려줌
		std::string	_proxy_authenticate; //요청한 서버가 프록시 서버인 경우 유저 인증을 위한 값
		std::string	_set_cookie; //서버측에서 클라이언트에게 세션 쿠키 정보를 설정
};

#endif