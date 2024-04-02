#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

#define HTTP_VERSION "HTTP/1.1"

class Response {
	public:
		Response();
		Response(short int status);
		Response(short int status, std::string bodyFile);
		~Response();
		std::string	getFullReponse();

	private:
		short int	_status;
		std::vector<std::string>	_header;
		std::string	_bodyFile;
		std::string	_body;
		void	_success();
};

template<typename T>
std::string	toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

