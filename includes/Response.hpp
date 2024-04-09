#pragma once

#include <iostream>
#include <vector>
#include <ctime>
#include <map>
#include <dirent.h>
#include <sys/types.h>
#include <iomanip>
#include "utils.hpp"
#include "errorPages.hpp"
#include "ServerConfig.hpp"

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
		Response(int status);
		Response(int status, std::string bodyFile);
		Response	&operator=(const Response &other);
		size_t		size() const;
		const char	*response() const;
		std::string	getFullResponse() const;
		std::string	getContentType(const std::string &filename) const;
		std::string	getStatusMessage(int status) const;
		void		defineStatusLine(int status);
		void		addNewField(std::string key, std::string value);
		void		generateFullResponse();
};
