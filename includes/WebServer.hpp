#pragma once

#include "includeAll.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"

#define GIGA_LIMIT 184467e10
#define MEGA_LIMIT 1844674e13
#define KILO_LIMIT 1844674e16
#define ONE_GIGA 1000000000
#define ONE_MEGA 1000000
#define ONE_KILO 1000

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

class WebServer {
	public:
		std::vector<ServerConfig>	servers;

		void	setupConfig(char* filename);
		static bool	configIsValid(char* filename);
};

std::map<std::string, Server::Keywords>	buildServerMap();
std::map<std::string, Route::Keywords>	buildRouteMap();
void	checkInsideRoute(std::ifstream& file, std::string& line)
	throw(std::runtime_error);
bool	invalidServerInputs(std::ifstream& file,
	std::string& line,
	bool* serverBrackets,
	std::map<std::string, Server::Keywords>& serverMap);
void	addRoutes(std::ifstream& file, std::string& line, ServerConfig& server);
void	addServers(std::ifstream& file, std::vector<ServerConfig>& servers);

std::ostream&	operator<<(std::ostream& o, WebServer& webserv);
