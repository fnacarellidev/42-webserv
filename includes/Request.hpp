#pragma once
#include <iostream>
#include <list>
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
		std::list<ServerConfig> _serverConfigs;
	public:
		Methods method;
		std::string filePath;
		Request(std::string request, std::list<ServerConfig> serverConfigs);
};
