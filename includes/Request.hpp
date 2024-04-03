#pragma once
#include <iostream>
#include "../includes/HttpStatus.hpp"


enum Methods {
	GET,
	POST,
	DELETE,
	INVALMETHOD
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
		std::string _rootPath;
		std::string _fileContent;
	public:
		Request(std::string request, std::string rootPath);
		void setStatusCode(HttpStatus::Code httpStatusCode);
		HttpStatus::Code getStatusCode();
		Methods getMethod();
		std::string getRootPath();
		void setFileContent(std::string fileContent);
		std::string getFileContent();
};
