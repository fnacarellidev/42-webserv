#pragma once

#include <exception>
#include <vector>
#include <map>
#include "RouteConfig.hpp"
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

		ServerConfig&	findByHostNamePort(std::string const& host, std::string const* names, size_t const size, unsigned int const port) const throw(ServerNotFound);
		void	addServers(const char* filename) throw (std::runtime_error);
		bool	configIsValid(const char* filename);
};

std::map<std::string, Server::Keywords>	buildServerMap();
std::map<std::string, Route::Keywords>	buildRouteMap();
void	checkInsideRoute(std::ifstream& file, std::string& line) throw(std::runtime_error);
ServerConfig*	searchViaName(std::string const name, unsigned int const port, std::vector<ServerConfig>& servers);
ServerConfig&	searchViaHost(std::string const& host, unsigned int const port, std::vector<ServerConfig>& servers) throw(ServerNotFound);
bool	invalidServerInputs(std::ifstream& file, std::string& line, bool* serverBrackets, std::map<std::string, Server::Keywords>& serverMap);
void	addRoutes(std::ifstream& file, ServerConfig& server);

std::ostream&	operator<<(std::ostream& o, const Config& webserv);
