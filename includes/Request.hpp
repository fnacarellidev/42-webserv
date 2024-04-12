#pragma once
#include <iostream>
#include <list>
#include <unistd.h>
#include "Response.hpp"
#include "ServerConfig.hpp"

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
		std::vector<ServerConfig> _serverConfigs;
		Response runGet();
		bool shouldRedirect();
		RouteConfig* _route;
		std::string _reqUri;
		bool _dirListEnabled;
		/* Response runPost(); */
		/* Response runDelete(); */
	public:
		Methods method;
		std::string file;
		std::string filePath;
		Response runRequest();
		Request(std::string request, std::vector<ServerConfig> serverConfigs);
};
