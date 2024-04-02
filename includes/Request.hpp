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

namespace HttpStatus {
	enum Code {
		OK = 200,
		NOTALLOWED = 405
	};
};

class Request {
	private:
		Methods _method;
		HttpStatus::Code _httpStatusCode;
	public:
		Request(std::string request);
		void setStatusCode(HttpStatus::Code httpStatusCode);
};
