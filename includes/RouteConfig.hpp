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

class RouteConfig {
	public:
		std::string	_path;
		std::pair<std::string, std::string>	_redirect;
		std::vector<std::string>	_index;
		std::string	_root;
		unsigned short	_acceptMethodsBitmask;
		bool	_dirList;

		RouteConfig();
		bool getDirList() const;
		std::string getPath() const;
		std::string getRoot() const;
		std::vector<std::string> getIndex() const;
		std::pair<std::string, std::string> getRedirect() const;
		unsigned short getAcceptMethodsBitmask() const;
		void setRoot(std::string root);
		void setPath(std::string path);
		void setIndex(std::vector<std::string> index);
		void setDirList(bool dirList);
		void setRedirect(std::pair<std::string, std::string> redirect);
		void setAcceptMethodsBitmask(unsigned short acceptMethodsBitmask);
};
