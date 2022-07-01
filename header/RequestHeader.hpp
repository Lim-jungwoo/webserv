#ifndef REQUESTHEADER_HPP
# define REQUESTHEADER_HPP

# include "EntityHeader.hpp"

class RequestHeader : public EntityHeader
{//HTTP요청에서 사용되지만 메시지의 컨텐츠와 관련이 없는 패치될 리소스나 클라리언트 자체에 대한 자세한 정보를 포함하는 헤더
	public:
		RequestHeader() {}
		RequestHeader(const RequestHeader& rh) {}
		virtual ~RequestHeader() {}

		RequestHeader&	operator=(const RequestHeader& rh) { return (*this); }

	private:
		std::string	_host; //요청하려는 서버 호스트 이름과 포트 번호
		std::string	_user_agent; //현재 사용자가 어떤 클라리언트(운영체제, 브라우저 등)을 통해 요청을 보냈는지 알 수 있다
		std::string	_accept; //클라이언트가 허용할 수 있는 파일 형식(MIME TYPE)
		std::string	_accept_charset; //클라이언트가 지원가능한 문자열 인코딩 방식
		std::string	_accept_language; //클라이언트가 지원가능한 언어 나열
		std::string	_accept_encoding; //클라이언트가 해석가능한 압축 방식 지정
		std::string	_origin; //POST같은 요청을 보낼 때, 요청이 어느 주소에서 시작되었는지를 나타냄, 경로 정보는 포함하지 않고 서버 이름만 포함
		std::string	_authorization; //인증 토큰을 서버로 보낼 때 사용하는 헤더

		//사용안할 것 같은 것
		std::string	_referer; //현재 페이지로 연결되는 링크가 있던 이전 웹 페이지의 주소
		std::string	_cookie; //Set-Cookie헤더와 함께 서버로부터 이전에 전송됐던 저장된 HTTP 쿠키 포함
		std::string	_if_modified_since; //여기에 쓰여진 시간 이후로 변경된 리소스를 취득하고 캐시가 만료되었을 때에만 데이터를 전송하는데 사용
};

#endif