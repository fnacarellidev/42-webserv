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
#include "../includes/utils.hpp"

#define SERVER_NAME "webserver/1.0"
#define HTTP_VERSION "HTTP/1.1"

typedef std::pair<std::string, std::string> t_fields;

class Response {
	private:
		short int					_status;
		std::string					_statusLine;
		std::vector<t_fields>		_header;
		std::string					_body;
		std::string					_bodyFile;
		void	_success();
		void	_redirection();
		void	_error();
		void	_serverError();
	public:
		Response();
		Response(short int status);
		Response(short int status, std::string bodyFile);
		~Response();
		std::string	getFullReponse();
};
