#pragma once

#include <iostream>
#include <sstream>
#include <vector>

/*
 * individual server config
 * for each `server {}` inside config will be stored here
*/
class ServerConfig
{
	private:
		std::vector< std::string >	_allServerNames;
		int	port;
	public:
		std::vector< std::string > getServerNames() const;
};
