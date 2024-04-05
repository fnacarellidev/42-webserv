#pragma once

#include <exception>
#include <vector>
#include "ServerConfig.hpp"

namespace Server
{
	enum Keywords
	{
		SERVER = 1,
		HOST,
		PORT,
		NAMES,
		LIMIT,
		ERROR,
		ROUTE
	};
};

namespace Route
{
	enum Keywords
	{
		ROUTE = 1,
		INDEX,
		REDIRECT,
		ROOT,
		METHODS,
		LISTING,
		CGI
	};
};

class ServerNotFound: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class Config
{
	public:
		std::vector<ServerConfig>	servers;

		ServerConfig&	findByHostNamePort(std::string const& host,\
			std::string const* names, size_t const size, unsigned int const port) \
			const throw(ServerNotFound);
		void	addServers(const char* filename) throw (std::runtime_error);
		bool	configIsValid(const char* filename);
};
