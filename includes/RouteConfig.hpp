#pragma once

#include <iostream>
#include <vector>
#include <utility>

#define GET_OK 0b0001
#define POST_OK 0b0010
#define DELETE_OK 0b0100

#define DEFAULT_ROOT "/var/www/html"
#define DEFAULT_INDEX "index.html"
#define DEFAULT_DIRLIST false
#define DEFAULT_ACCEPT_METHODS GET_OK

class RouteConfig
{
	private:
		std::pair<std::string, std::string>	_redirect;
		std::vector<std::string>	_index;
		std::string	_root;
		unsigned short	_acceptMethodsBitmask;
		bool	_dirList;
	public:
		RouteConfig();
		bool getDirList();
		std::string getRoot();
		std::vector<std::string> getIndex();
		std::pair<std::string, std::string> getRedirect();
		unsigned short getAcceptMethodsBitmask();
		void setRoot(std::string root);
		void setIndex(std::vector<std::string> index);
		void setDirList(bool dirList);
		void setRedirect(std::pair<std::string, std::string> redirect);
		void setAcceptMethodsBitmask(unsigned short acceptMethodsBitmask);
};
