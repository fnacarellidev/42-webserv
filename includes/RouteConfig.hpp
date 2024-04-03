#pragma once

#include <iostream>
#include <utility>

class RouteConfig
{
	private:
		std::pair< std::string, std::string >	_redirect;
		std::string*	_index;
		std::string	_root;
		unsigned short	_allowedMethodsBitmask;
		bool	_dirList;
	public:
		RouteConfig();
		std::string getRoot() { return _root; };
		unsigned short getAllowedMethodsBitmask() { return _allowedMethodsBitmask; };
};
