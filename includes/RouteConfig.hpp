#pragma once

#include <iostream>
#include <utility>

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
