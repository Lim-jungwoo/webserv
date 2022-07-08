#ifndef ENTITYHEADER_HPP
# define ENTITYHEADER_HPP

# include "GeneralHeader.hpp"

class EntityHeader : public GeneralHeader
{//컨텐츠 길이나 MIME 타입과 같이 entity 바디에 대한 자세한 정보를 포함하는 헤더
	public:
		EntityHeader() {}
		EntityHeader(const EntityHeader& eh) { (void)eh; }
		virtual ~EntityHeader() {}

		EntityHeader&	operator=(const EntityHeader& eh) { (void)eh; return (*this); }

		void	setContentLength(const size_t& size = 0)
		{ this->_content_length = intToStr(size); }
		void	setContentType(const std::string& path, std::string type = "")
		{//html, css, js, jpeg, png, bmp, plain만 정해놨는데 다른 것도 해야되는지 알아봐야 한다.
			if (type != "")
			{
				this->_content_type = type;
				return ;
			}
			type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
			if (type == "css")
				this->_content_type = "text/css";
			else if (type == "html")
				this->_content_type = "text/html";
			else if (type == "js")
				this->_content_type = "text/javascript";
			else if (type == "apng")
				this->_content_type = "image/apng";
			else if (type == "avif")
				this->_content_type = "image/avif";
			else if (type == "gif")
				this->_content_type = "image/gif";
			else if (type == "jpeg" || type == "jpg")
				this->_content_type = "image/jpeg";
			else if (type == "png")
				this->_content_type = "image/png";
			else if (type == "svg")
				this->_content_type = "image/svg+xml";
			else if (type == "webp")
				this->_content_type = "image/webp";
			//bmp는 사용을 안 하는 것이 좋다고 한다.
			// else if (type == "bmp")
			// 	this->_content_type = "image/bmp";
			//비디오, 음악 파일은 일단 제외했다.
			else
				this->_content_type = "text/plain";
		}
		void	setContentLanguage(const std::string& lang = "en")
		{ this->_content_language = lang;}
		void	setContentLocation(const std::string& path)
		{ this->_content_location = path; }
		
		
		//둘 다 쓰는지 확인해보자
		void	setAllow(const std::string& allow)
		{ this->_allow = allow; }
		void	setAllow(const std::set<std::string>& methods)
		{
			std::set<std::string>::iterator	it = methods.begin();
			while (it != methods.end())
			{
				this->_allow = *it;
				this->_allow += ", ";
				it++;
			}
		}
	
	// protected:
		std::string	_content_length; //메시지의 크기
		std::string	_content_type; //컨텐츠의 타입(MIME)과 문자열 인코딩(utf-8등)을 명시
		//ex) Content-Type: text/html; charset=utf-8
		std::string	_content_language; //사용자의 언어
		std::string	_content_location; //반환된 데이터 개체의 실제 위치
		std::string	_content_encoding; //컨텐츠가 압축된 방식
		std::string	_allow; //지원되는 HTTP 메소드, 만약 비어있다면 모든 메소드가 금지라는 뜻

		/*
		//사용안할 것 같은 것
		std::string	_content_disposition; //응답 본문을 브라우저가 어떻게 표시해야할 지 알려주는 헤더
		std::string	_content_security_policy; //다른 외부 파일들을 불러오는 경우, 차단할 소스와 불러올 소스를 명시
		std::string	_expires; //자원의 만료 일자
		std::string	_etag; //리소스의 버전을 식별하는 고유한 문자열 검사기(주로 캐시 확인용으로 사용)
		*/
};

#endif