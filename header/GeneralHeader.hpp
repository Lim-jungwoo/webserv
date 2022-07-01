#ifndef GENERALHEADER_HPP
# define GENERALHEADER_HPP

# include "../includes/Utils.hpp"

class GeneralHeader
{//요청과 응답 모두에 적용되지만 바디에서 최종적으로 전송되는 데이터와는 관련이 없는 헤더
	public:
		GeneralHeader() {}
		GeneralHeader(const GeneralHeader& gh) {}
		virtual ~GeneralHeader() {}

		GeneralHeader&	operator=(const GeneralHeader& gh) { return (*this); }
		// virtual void abstract() = 0;

	private:
		std::string	_date; //HTTP메시지가 만들어진 시각
		std::string	_connection; //일반적으로 HTTP/1.1을 사용 ex) Connection: keep-alive
		std::string	_transfer_encoding; //사용자에게 entity를 안전하게 전송하기 위해 사용하는 인코딩 형식을 지정


		//사용안 할 거 같은 것들
		std::string	_pragma; //캐시제어
		std::string	_cache_control; //캐시를 제어할 때 사용
		std::string	_upgrade; //프로토콜 변경시 사용
		std::string	_via; //중계(프록시)서버의 이름, 버전, 호스트명
};

#endif