#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

# include "EntityHeader.hpp"

class ResponseHeader : public EntityHeader
{
	public:
		ResponseHeader() {}
		ResponseHeader(const ResponseHeader& rh) {}
		virtual ~ResponseHeader() {}

		ResponseHeader&	operator=(const ResponseHeader& rh) { return (*this); }
	
	private:
		std::string	_server; //웹서버 정보
		std::string	_www_authenticate; //사용자 인증이 필요한 자원을 요구할 시, 서버가 제공하는 인증 방식

		//사용안할 것 같은 것
		std::string	_access_control_allow_origin; //요청 Host와 응답 Host가 다를 때 CORS에러를 막기 위해 사용
		std::string	_age; //시간이 얼마나 흘렀는지 초 단위로 알려줌
		std::string	_referrer_policy; //서버 referrer정책을 알려줌
		std::string	_proxy_authenticate; //요청한 서버가 프록시 서버인 경우 유저 인증을 위한 값
		std::string	_set_cookie; //서버측에서 클라이언트에게 세션 쿠키 정보를 설정
};

#endif