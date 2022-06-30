#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

#include "../server/utils.hpp"

class ResponseHeader
{
	public:
		ResponseHeader() {}
		ResponseHeader(const ResponseHeader& response_header) {}
		virtual ~ResponseHeader() {}

		ResponseHeader&	operator=(const ResponseHeader& response_header) { return (*this); }

		void	initErrorMap()
		{
			this->_error_map[ErrorCode::Continue] = "Continue";
			this->_error_map[ErrorCode::OK] = "OK";
			this->_error_map[ErrorCode::Created] = "Created";
			this->_error_map[ErrorCode::No_Content] = "No Content";
			this->_error_map[ErrorCode::Bad_Request] = "Bad Request";
			this->_error_map[ErrorCode::Forbidden] = "Forbidden";
			this->_error_map[ErrorCode::Not_Found] = "Not Found";
			this->_error_map[ErrorCode::Payload_Too_Large] = "Payload Too Large";
			this->_error_map[ErrorCode::Internal_Server_error] = "Internal Server Error";
		}

	private:
		std::map<int, std::string>	_error_map;
		std::string					_allow;
		std::string					_contentLanguage;
		std::string					_contentLength;
		std::string					_contentLocation;
		std::string					_contentType;
		std::string					_date;
		std::string					_lastModified;
		std::string					_location;
		std::string					_retryAfter;
		std::string					_server;
		std::string					_transferEncoding;
		std::string					_wwwAuthenticate;
};

#endif