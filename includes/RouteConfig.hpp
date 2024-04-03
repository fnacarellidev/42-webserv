#pragma once

#include <iostream>
#include <utility>

enum RouteMask
{
	MASK_GET = 1,
	MASK_POST = 1<<1,
	MASK_DELETE = 1<<2
};

class RouteConfig
{
	private:
		// sem default?
		std::pair< std::string, std::string >	_redirect;
		// default index.html
		std::string*	_index;
		// default /?
		std::string	_root;
		// default so get 0 = none, 1 = get, 1<<1 = 
		char	_acceptMethodsBitmask;
		// default pra false
		bool	_dirList;
	public:
};
