#pragma once
#include <iostream>
#include <list>
#include <sstream>
#include <cstdlib>
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
		std::string	_fullRequest;
		std::vector<ServerConfig> _serverConfigs;
		Response runGet();
		Response runPost();
		/* Response runDelete(); */
	public:
		Methods method;
		std::string filePath;
		Response runRequest();
		Request(std::string request, std::vector<ServerConfig> serverConfigs);
};
