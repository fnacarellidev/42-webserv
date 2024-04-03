#include "../includes/Config.hpp"
#include "../includes/utils.hpp"
#include <vector>
#include <fstream>

static bool	invalidServerInputs(std::ifstream& file, \
	std::string& line, \
	bool* openBrackets, \
	std::map< std::string, ServerKeywords >& serverMap)
{
	if (line.find_first_of('\t') != std::string::npos)
		return true;

	while (!file.eof()) {
		std::vector< std::string > splited = split(line, ' ');
		for (size_t i = 0; i < splited.size(); i++) {
			if (serverMap.find(splited[0]) == serverMap.end())
				return true;
			switch (serverMap.find(splited[0])->second) {
				SERVER:
					if (splited.size() != 2 || splited[1] != "{" || *openBrackets)
						return true;
					*openBrackets = !*openBrackets;
				HOST: PORT: LIMIT:
					if (splited.size() != 2 || \
						splited[1].find_first_of(';') == std::string::npos)
						return true;
				NAMES: // TODO!

				ERROR: // TODO!

				ROUTE: // TODO!
					if (splited.size() != 2 || splited[1] != "{")
						try {
							checkInsideRoute(file, line, )
						}
				default:

			}
		}
		std::getline(file, line);
	}
	return false;
}

static ServerConfig&	searchViaHost(std::string const& host, \
	unsigned int const port, \
	std::list< ServerConfig >& servers)
throw(ServerNotFound)
{
	std::list< ServerConfig >::iterator end(servers.end());
	std::list< ServerConfig >::iterator begin(servers.begin());

	while (begin != end) {
		if (begin->getHost() == host && begin->getPort() == port)
			return *begin;
		++begin;
	}
	throw ServerNotFound();
}

static ServerConfig*	searchViaName(std::string const name, \
	unsigned int const port, \
	std::list< ServerConfig >& servers)
{
	std::list< ServerConfig >::iterator end(servers.end());
	std::list< ServerConfig >::iterator begin(servers.begin());

	while (begin != end) {
		for (size_t i = 0; i < begin->getSizeNames(); i++) {
			if (begin->getNames()[i] == name && begin->getPort() == port)
				return &(*begin);
		}
		++begin;
	}

	return NULL;
}

static std::map< std::string, ServerKeywords >	buildServerMap()
{
	std::map< std::string, ServerKeywords > map;

	map["server"] = SERVER;
	map["host"] = HOST;
	map["port"] = PORT;
	map["names"] = NAMES;
	map["limit"] = LIMIT;
	map["error"] = ERROR;
	map["route"] = ROUTE;

	return map;
}

static std::map< std::string, RouteKeywords >	buildRouteMap()
{
	std::map< std::string, RouteKeywords > map;

	map["index"] = INDEX;
	map["redirect"] = REDIRECT;
	map["root"] = ROOT;
	map["methods"] = METHODS;
	map["listing"] = LISTING;
	map["cgi"] = CGI;

	return map;
}
