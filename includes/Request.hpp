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
		RouteConfig* _route;
		bool _dirListEnabled;
		HttpStatus::Code runGet();
		HttpStatus::Code runPost();
		HttpStatus::Code runDelete();
	public:
		Methods method;
		bool _shouldRedirect;
		std::string	_reqUri;
		std::string _locationHeader;
		std::string filePath;
		std::string	root;
		ServerConfig _server;
		Response runRequest();
		void initRequest(std::string &request);
		Request(std::string request, std::vector<ServerConfig> serverConfigs);
};
