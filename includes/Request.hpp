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
		RouteConfig* _route;
		bool _dirListEnabled;
		int runGet();
		int runPost();
		/* Response runDelete(); */
	public:
		Methods method;
		bool _shouldRedirect;
		std::string	_reqUri;
		std::string _locationHeader;
		std::string filePath;
		std::string	root;
		ServerConfig _server;
		Response runRequest();
		Request(std::string request, std::vector<ServerConfig> serverConfigs);
};
