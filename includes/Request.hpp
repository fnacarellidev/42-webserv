#pragma once
#include <iostream>

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
	public:
		Request(std::string request);
};
