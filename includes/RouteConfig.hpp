#pragma once

#include <iostream>
#include <vector>
#include <utility>

#define GET_OK 0b0001
#define POST_OK 0b0010
#define DELETE_OK 0b0100

class RouteConfig
{
	private:
		// sem default?
		std::pair<std::string, std::string>	_redirect;
		// default index.html
		std::vector<std::string>	_index;
		// default /?
		std::string	_root;
		unsigned short	_acceptMethodsBitmask;
		// default pra false
		bool	_dirList;
	public:
		RouteConfig();
		bool getDirList();
		std::string getRoot();
		std::vector< std::string > getIndex();
		std::pair<std::string, std::string> getRedirect();
		unsigned short getAcceptMethodsBitmask();
		void setRoot(std::string root);
		void setIndex(std::vector< std::string > index);
		void setDirList(bool dirList);
		void setRedirect(std::pair<std::string, std::string> redirect);
		void setAcceptMethodsBitmask(unsigned short acceptMethodsBitmask);
};
