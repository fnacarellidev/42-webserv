#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <ctime>
#include <sys/stat.h>
#include <map>

#define SERVER_NAME "webserver/1.0"
#define HTTP_VERSION "HTTP/1.1"

typedef std::pair<std::string, std::string> t_fields;

class Response {
	public:
		Response();
		Response(short int status);
		Response(short int status, std::string bodyFile);
		~Response();
		std::string	getFullReponse();

	private:
		short int					_status;
		std::string					_statusLine;
		std::vector<t_fields>		_header;
		std::string					_body;
		std::string					_bodyFile;
		void	_success();
};

template<typename T>
std::string	toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

