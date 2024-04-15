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

class Request {
	private:
		std::string	_fullRequest;
		ServerConfig _server;
		Response runGet();
		Response runPost();
		bool _shouldRedirect;
		RouteConfig* _route;
		std::string _reqUri;
		bool _dirListEnabled;
		Response runDelete();
	public:
		Methods method;
		std::string file;
		std::string filePath;
		Response runRequest();
		Request(std::string request, std::vector<ServerConfig> serverConfigs);
};
