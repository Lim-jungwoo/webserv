#ifndef ENTITYHEADER_HPP
# define ENTITYHEADER_HPP

# include "GeneralHeader.hpp"

class EntityHeader : public GeneralHeader
{//컨텐츠 길이나 MIME 타입과 같이 entity 바디에 대한 자세한 정보를 포함하는 헤더
	public:
		EntityHeader() {}
		EntityHeader(const EntityHeader& eh) {}
		virtual ~EntityHeader() {}

		EntityHeader&	operator=(const EntityHeader& eh) { return (*this); }
	
	private:
		std::string	_content_length; //메시지의 크기
		std::string	_content_type; //컨텐츠의 타입(MIME)과 문자열 인코딩(utf-8등)을 명시
		//ex) Content-Type: text/html; charset=utf-8
		std::string	_content_language; //사용자의 언어
		std::string	_content_location; //반환된 데이터 개체의 실제 위치
		std::string	_content_encodig; //컨텐츠가 압축된 방식
		std::string	_location; //300번대 응답이나 201(created)응답일 때 어느 페이지로 이동할 지 알려주는 헤더
		std::string	_last_modified; //리소스의 마지막 수정 날짜
		std::string	_allow; //지원되는 HTTP 요청 메소드

		//사용안할 것 같은 것
		std::string	_content_disposition; //응답 본문을 브라우저가 어떻게 표시해야할 지 알려주는 헤더
		std::string	_content_security_policy; //다른 외부 파일들을 불러오는 경우, 차단할 소스와 불러올 소스를 명시
		std::string	_expires; //자원의 만료 일자
		std::string	_etag; //리소스의 버전을 식별하는 고유한 문자열 검사기(주로 캐시 확인용으로 사용)
};

#endif