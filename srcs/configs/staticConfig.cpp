#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include <map>
#include <vector>
#include <fstream>
#include <exception>

static bool	serverKeyMatch(Server::Keywords key);
static bool	routeKeyMatch(Route::Keywords key);

std::map<std::string, Server::Keywords>	buildServerMap()
{
	std::map<std::string, Server::Keywords> map;

	map["server"] = Server::SERVER;
	map["host"] = Server::HOST;
	map["port"] = Server::PORT;
	map["names"] = Server::NAMES;
	map["limit"] = Server::LIMIT;
	map["error"] = Server::ERROR;
	map["route"] = Server::ROUTE;

	return map;
}

std::map<std::string, Route::Keywords>	buildRouteMap()
{
	std::map<std::string, Route::Keywords> map;

	map["route"] = Route::ROUTE;
	map["index"] = Route::INDEX;
	map["redirect"] = Route::REDIRECT;
	map["root"] = Route::ROOT;
	map["methods"] = Route::METHODS;
	map["listing"] = Route::LISTING;
	map["cgi"] = Route::CGI;

	return map;
}

void	checkInsideRoute(std::ifstream& file, std::string& line) \
throw(std::runtime_error)
{
	std::map<std::string, Route::Keywords> routeMap(buildRouteMap());
	bool routeBrackets = false;

	while (!file.eof()) {
		std::vector<std::string> splited = split(line, ' ');

		if (!splited.empty()) {
			std::map<std::string, Route::Keywords>::iterator found = \
				routeMap.find(splited[0]);

			if (splited.size() == 1 && splited[0] == "}" && routeBrackets)
				return ;
			if (splited.size() != 2)
				throw std::runtime_error("");
			if (found == routeMap.end())
				throw std::runtime_error("");

			Route::Keywords key = found->second;

			if (key == Route::ROUTE && (splited[1] != "{" || routeBrackets))
				throw std::runtime_error("");
			else if (routeKeyMatch(key) && \
				splited[1].find_first_of(';') == std::string::npos)
				throw std::runtime_error("");
			else if (key == Route::ROUTE)
				routeBrackets = !routeBrackets;
		}
		line.clear();
		std::getline(file, line);
		trim(line, "\t \n");
	}
}

bool	invalidServerInputs(std::ifstream& file, \
	std::string& line, \
	bool* serverBrackets, \
	std::map<std::string, Server::Keywords>& serverMap)
{
	while (!file.eof()) {
		std::vector<std::string> splited = split(line, ' ');

		if (!splited.empty()) {
			std::map<std::string, Server::Keywords>::iterator found = \
				serverMap.find(splited[0]);

			if (splited.size() == 1 && splited[0] == "}" && *serverBrackets) {
				*serverBrackets = !*serverBrackets;
				return false;
			}
			if (splited.size() != 2)
				return true;
			if (found == serverMap.end())
				return true;

			Server::Keywords key = found->second;

			if (key == Server::SERVER && (splited[1] != "{" || *serverBrackets))
				return true;
			else if (serverKeyMatch(key) && \
				splited[1].find_first_of(';') == std::string::npos)
				return true;
			else if (key == Server::ROUTE && splited[1] == "{") {
				try {
					checkInsideRoute(file, line);
				} catch (std::exception& e) {
					return true;
				}
			} else if (key == Server::SERVER)
				*serverBrackets = !*serverBrackets;
		}
		line.clear();
		std::getline(file, line);
		trim(line, "\t \n");
	}
	return false;
}

ServerConfig&	searchViaHost(std::string const& host, \
	unsigned int const port, \
	std::vector<ServerConfig>& servers)
throw(ServerNotFound)
{
	std::vector<ServerConfig>::iterator end(servers.end());
	std::vector<ServerConfig>::iterator begin(servers.begin());

	while (begin != end) {
		if (begin->getHost() == host && begin->getPort() == port)
			return *begin;
		++begin;
	}
	throw ServerNotFound();
}

ServerConfig*	searchViaName(std::string const name, \
	unsigned int const port, \
	std::vector<ServerConfig>& servers)
{
	std::vector<ServerConfig>::iterator end(servers.end());
	std::vector<ServerConfig>::iterator begin(servers.begin());

	while (begin != end) {
		for (size_t i = 0; i < begin->getSizeNames(); i++) {
			if (begin->getNames()[i] == name && begin->getPort() == port)
				return &(*begin);
		}
		++begin;
	}

	return NULL;
}

static bool	serverKeyMatch(Server::Keywords key)
{
	return key == Server::HOST || key == Server::PORT || key == Server::NAMES || \
		key == Server::LIMIT || key == Server::ERROR;
}

static bool	routeKeyMatch(Route::Keywords key)
{
	return key == Route::INDEX || key == Route::REDIRECT || key == Route::ROOT || \
		key == Route::METHODS || key == Route::LISTING;
}
