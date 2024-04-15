#pragma once

#include <iostream>
#include <vector>
#include <utility>

#define NONE_OK 0b0000
#define GET_OK 0b0001
#define POST_OK 0b0010
#define DELETE_OK 0b0100

#define DEFAULT_ROOT "/var/www/html"
#define DEFAULT_INDEX "index.html"
#define DEFAULT_DIRLIST false
#define DEFAULT_ACCEPT_METHODS GET_OK
#define DEFAULT_PATH "/"

class RouteConfig {
	public:
		std::string	path;
		std::pair<std::string, std::string>	redirect;
		std::vector<std::string>	index;
		std::string	root;
		unsigned short	acceptMethodsBitmask;
		bool	dirList;

		RouteConfig();
};

std::ostream&	operator<<(std::ostream& os, RouteConfig& routeConfig);
