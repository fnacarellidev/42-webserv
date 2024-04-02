#pragma once
#include <iostream>

enum Methods {
	GET,
	POST,
	DELETE,
	INVALMETHOD
};

class Request {
	private:
		unsigned int _method;
	public:
		Request(std::string request);
};
