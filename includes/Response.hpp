#pragma once

#include <fcntl.h>
#include "includeAll.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include "errorPages.hpp"
#include "ServerConfig.hpp"

typedef std::pair<std::string, std::string> t_fields;

class Request;

class Response {
	private:
		short int					_status;
		std::string					_statusLine;
		std::vector<t_fields>		_headerFields;
		std::string					_body;
		std::string					_filePath;
		std::string					_requestUri;
		std::string					_locationHeader;
		std::string					_errPage;
		std::map<int, std::string>			_statusMessages;
		std::map<std::string, std::string>	_mimeTypes;
		std::string					_fullResponse;
		std::vector<std::string>	_allowedCgis;
		int	_connectionFd;
		void	_success();
		void	_redirection();
		void	_error();
		void	_serverError();
	public:
		Response(int status);
		Response(int status, Request &request);
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
