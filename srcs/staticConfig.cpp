#include "../includes/Config.hpp"
#include "../includes/utils.hpp"
#include <vector>
#include <fstream>
#include <exception>


std::map<std::string, ServerKeywords>	buildServerMap()
{
	std::map<std::string, ServerKeywords> map;

	map["server"] = SERVER;
	map["host"] = HOST;
	map["port"] = PORT;
	map["names"] = NAMES;
	map["limit"] = LIMIT;
	map["error"] = ERROR;
	map["route"] = ROUTE;

	return map;
}

std::map<std::string, RouteKeywords>	buildRouteMap()
{
	std::map<std::string, RouteKeywords> map;

	map["route"] = IROUTE;
	map["index"] = INDEX;
	map["redirect"] = REDIRECT;
	map["root"] = ROOT;
	map["methods"] = METHODS;
	map["listing"] = LISTING;
	map["cgi"] = CGI;

	return map;
}

void	checkInsideRoute(std::ifstream& file, std::string& line) \
throw(std::runtime_error)
{
	std::map<std::string, RouteKeywords> routeMap(buildRouteMap());
	bool routeBrackets = false;

	while (!file.eof()) {
		std::vector<std::string> splited = split(line, ' ');

		if (splited.size() == 1 && splited[0] == "}" && routeBrackets)
			return ;
		else if (splited[0] == "}") // n sei o q tava fazendo aqui
			return ;
		if (routeMap.find(splited[0]) == routeMap.end())
			throw std::runtime_error("");
		switch (routeMap.find(splited[0])->second) {
			case IROUTE:
				if (splited.size() != 2 || splited[1] != "{" || routeBrackets)
					throw std::runtime_error("");
				routeBrackets = !routeBrackets;
				break;
			case INDEX: case REDIRECT: case ROOT: case METHODS: case LISTING:
				if (splited.size() != 2 || \
					splited[1].find_first_of(';') == std::string::npos)
					throw std::runtime_error("");
				break;
			case CGI:
			default: ;// nada acontece feijoada
		}
		line.clear();
		std::getline(file, line);
		trim(line, "\t \n");
	}
}

bool	invalidServerInputs(std::ifstream& file, \
	std::string& line, \
	bool* serverBrackets, \
	std::map<std::string, ServerKeywords>& serverMap)
{
	while (!file.eof()) {
		std::vector<std::string> splited = split(line, ' ');

		if (!splited.empty()) {
			if (splited[0] == "}" && *serverBrackets) {
				*serverBrackets = !*serverBrackets;
				return false;
			} else if (splited[0] == "}") // n sei o q tava fazendo aqui
				return false;
			if (serverMap.find(splited[0]) == serverMap.end())
				return true;
			switch (serverMap.find(splited[0])->second) {
				case SERVER:
					if (splited.size() != 2 || splited[1] != "{" || *serverBrackets)
						return true;
					*serverBrackets = !*serverBrackets;
					break;
				case HOST: case PORT: case LIMIT: case NAMES: case ERROR:
					if (splited.size() != 2 || \
						splited[1].find_first_of(';') == std::string::npos)
						return true;
					break;
				case ROUTE:
					if (!(splited.size() != 2 || splited[1] != "{")) {
						try {
							checkInsideRoute(file, line);
						} catch (std::exception& e) {
							return true;
						}
					}
				default: ;// nada acontece feijoada
			}
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
