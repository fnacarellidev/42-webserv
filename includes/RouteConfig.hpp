#pragma once

#include <iostream>
#include <utility>

#define GET_OK 0b0001
#define POST_OK 0b0010
#define DELETE_OK 0b0100

class RouteConfig
{
	private:
		std::pair< std::string, std::string >	_redirect;
		std::string	_root;
		unsigned short	_allowedMethodsBitmask;
	public:
		RouteConfig();
		std::string getRoot() { return _root; };
		unsigned short getAllowedMethodsBitmask() { return _allowedMethodsBitmask; };
};
