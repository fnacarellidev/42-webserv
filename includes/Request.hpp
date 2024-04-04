#pragma once
#include <iostream>
#include <list>
#include "../includes/HttpStatus.hpp"
#include "../includes/ServerConfig.hpp"

#define GET_OK 0b0001
#define POST_OK 0b0010
#define DELETE_OK 0b0100

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
	public:
		Methods _method;
		std::string filePath;
		bool isDir;
		std::list<ServerConfig> _serverConfigs;
	public:
		Request(std::string request, std::list<ServerConfig> serverConfigs);
};
