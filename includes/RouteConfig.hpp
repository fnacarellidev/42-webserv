#pragma once

#include <iostream>
#include <utility>

#ifndef GET_OK
 #define GET_OK 0b0001
#endif

#ifndef POST_OK
 #define POST_OK 0b0010
#endif

#ifndef DELETE_OK
 #define DELETE_OK 0b0100
#endif

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
