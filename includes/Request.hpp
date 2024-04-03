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
	private:
		Methods _method;
		HttpStatus::Code _httpStatusCode;
		std::string _fileContent;
		std::list<ServerConfig> _serverConfigs;
	public:
		Request(std::string request, std::list<ServerConfig> serverConfigs);
		HttpStatus::Code getStatusCode() const;
		Methods getMethod() const;
		std::string getFileContent() const;
		std::list<ServerConfig> getServerConfigs() const;
		void setStatusCode(HttpStatus::Code httpStatusCode);
		void setFileContent(std::string fileContent);
};
