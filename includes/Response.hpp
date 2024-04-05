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
	private:
		short int					_status;
		std::string					_statusLine;
		std::vector<t_fields>		_headerFields;
		std::string					_body;
		std::string					_bodyFile;
		std::map<int, std::string>			_statusMessages;
		std::map<std::string, std::string>	_mimeTypes;
		std::string					_fullResponse;
		void	_success();
		void	_redirection();
		void	_error();
		void	_serverError();
	public:
		Response();
		Response(short int status);
		Response(short int status, std::string bodyFile);
		~Response();
		std::string	response() const;
		std::string	getContentType(const std::string &filename) const;
		std::string	getStatusMessage(int status) const;
		void		defineStatusLine(int status);
		void		addNewField(std::string key, std::string value);
		void		generateFullResponse();
};

template<typename T>
std::string	toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}
