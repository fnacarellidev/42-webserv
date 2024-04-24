#pragma once
#include "includeAll.hpp"
#include "Response.hpp"
#include "ServerConfig.hpp"
#include "utils.hpp"

enum Methods {
	GET,
	POST,
	DELETE,
	UNKNOWNMETHOD
};

enum RequestLine {
	METHOD,
	REQUESTURI,
	PROTOCOLVER
};

class Response;

class Request {
	private:
		std::string _host;
		std::string	_contentType;
		size_t		_contentLength;
		std::string _body;
		std::string	_fullRequest;
		bool _dirListEnabled;
		int	_connectionFd;
		HttpStatus::Code runGet();
		HttpStatus::Code runPost();
		HttpStatus::Code runDelete();
	public:
		Methods method;
		bool _shouldRedirect;
		RouteConfig* route;
		std::string	_reqUri;
		std::string _locationHeader;
		std::string filePath;
		std::string	root;
		bool		execCgi;
		std::string	cgiOutput;
		std::string resContentType;
		ServerConfig _server;
		Response runRequest();
		Request(std::string request, std::vector<ServerConfig> serverConfigs, int connectionFd);
		void initRequest(std::string &request);
};
