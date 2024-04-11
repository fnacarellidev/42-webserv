#pragma once

#include <exception>
#include <vector>
#include <map>
#include <cctype>
#include <limits>
#include <algorithm>
#include <cerrno>
#include <fstream>
#include <cstdlib>
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"

#define GIGA_LIMIT 184467e10
#define MEGA_LIMIT 1844674e13
#define KILO_LIMIT 1844674e16

namespace Server {
	enum Keywords {
		SERVER = 1,
		HOST,
		PORT,
		NAMES,
		LIMIT,
		ERROR,
		ROUTE,
		ROOT
	};
};

namespace Route {
	enum Keywords {
		ROUTE = 1,
		INDEX,
		REDIRECT,
		ROOT,
		METHODS,
		LISTING,
		PATH,
		CGI
	};
};

class Config {
	public:
		std::vector<ServerConfig>	servers;

		ServerConfig&	findByHostNamePort(std::string const& host,
			std::string const* names,
			size_t const size,
			unsigned int const port)
			throw(std::runtime_error);
		void	addServers(char* filename) throw (std::runtime_error);
		bool	configIsValid(char* filename);
};

std::map<std::string, Server::Keywords>	buildServerMap();
std::map<std::string, Route::Keywords>	buildRouteMap();
void	checkInsideRoute(std::ifstream& file, std::string& line)
	throw(std::runtime_error);
ServerConfig*	searchViaName(std::string const name,
	unsigned int const port,
	std::vector<ServerConfig>& servers);
ServerConfig&	searchViaHost(std::string const& host,
	unsigned int const port,
	std::vector<ServerConfig>& servers)
	throw(std::runtime_error);
bool	invalidServerInputs(std::ifstream& file,
	std::string& line,
	bool* serverBrackets,
	std::map<std::string, Server::Keywords>& serverMap);
void	addRoutes(std::ifstream& file, std::string& line, ServerConfig& server);
bool	validateErrorConfig(std::string& errors);
bool	validateHostConfig(std::string& ip);
bool	validateLimitConfig(std::string& limit);
bool	validatePortConfig(std::string& port);
bool	validateRedirectConfig(std::string& redirect);
bool	validateMethodsConfig(std::string& methods);
void	addErrors(std::string const& error, ServerConfig& server);

std::ostream&	operator<<(std::ostream& o, const Config& webserv);
