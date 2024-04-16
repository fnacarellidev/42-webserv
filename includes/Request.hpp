#pragma once
#include <iostream>
#include <list>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
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
		std::string	_fullRequest;
		ServerConfig _server;
		RouteConfig* _route;
		bool _shouldRedirect;
		bool _dirListEnabled;
		Response runGet();
		Response runPost();
		/* Response runDelete(); */
	public:
		Methods method;
		std::string	_reqUri;
		std::string filePath;
		std::string	root;
		Response runRequest();
		Request(std::string request, std::vector<ServerConfig> serverConfigs);
};
