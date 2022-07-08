#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

#include "../includes/Utils.hpp"

class ResponseHeader
{
	public:
		ResponseHeader() {}
		ResponseHeader(const ResponseHeader& response_header) {}
		virtual ~ResponseHeader() {}

		ResponseHeader&	operator=(const ResponseHeader& response_header) { return (*this); }

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