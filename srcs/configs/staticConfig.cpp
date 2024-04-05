#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include <map>
#include <vector>
#include <fstream>
#include <exception>

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
		std::vector<std::string> names = begin->getNames();

		for (std::vector<std::string>::iterator it = names.begin(); \
			it != names.end(); it++) {
			if (*it == name && begin->getPort() == port)
				return &(*begin);
		}
		++begin;
	}

	return NULL;
}
