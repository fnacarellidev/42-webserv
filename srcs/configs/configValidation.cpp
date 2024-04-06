
#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include <fstream>

static bool	serverKeyMatch(Server::Keywords key)
{
	return key == Server::HOST || key == Server::PORT || key == Server::NAMES || key == Server::LIMIT || key == Server::ERROR;
}

static bool	routeKeyMatch(Route::Keywords key)
{
	return key == Route::INDEX || key == Route::REDIRECT || key == Route::ROOT || key == Route::METHODS || key == Route::LISTING;
}

// needed to validate:
// methods are acceptable by PDF
// dirlist is on or off
// problably some CGI things
void	checkInsideRoute(std::ifstream& file, std::string& line) throw(std::runtime_error)
{
	std::map<std::string, Route::Keywords> routeMap(buildRouteMap());
	bool routeBrackets = false;

	while (!file.eof()) {
		std::vector<std::string> splited = split(line, ' ');

		if (!splited.empty()) {
			std::map<std::string, Route::Keywords>::iterator found = routeMap.find(splited[0]);

			if (splited.size() == 1 && splited[0] == "}" && routeBrackets)
				return ;
			if (splited.size() != 2 || found == routeMap.end())
				throw std::runtime_error("");

			Route::Keywords key = found->second;

			if (key == Route::ROUTE && (splited[1] != "{" || routeBrackets))
				throw std::runtime_error("");
			else if (routeKeyMatch(key) && splited[1].find_first_of(';') == std::string::npos)
				throw std::runtime_error("");
			else if (key == Route::ROUTE)
				routeBrackets = !routeBrackets;
		}
		line.clear();
		std::getline(file, line);
		trim(line, "\t \n");
	}
}

bool	invalidServerInputs(std::ifstream& file, std::string& line, bool* serverBrackets, std::map<std::string, Server::Keywords>& serverMap)
{
	bool	error = false;

	while (!file.eof()) {
		std::vector<std::string> splited = split(line, ' ');

		if (!splited.empty()) {
			std::map<std::string, Server::Keywords>::iterator found = serverMap.find(splited[0]);

			if (splited.size() == 1 && splited[0] == "}" && *serverBrackets) {
				*serverBrackets = !*serverBrackets;
				return false;
			}
			if (splited.size() != 2 || found == serverMap.end())
				return true;

			Server::Keywords key = found->second;

			if (key == Server::SERVER && (splited[1] != "{" || *serverBrackets))
				return true;
			else if (key == Server::ROUTE && splited[1] == "{") {
				try {
					checkInsideRoute(file, line);
				} catch (std::exception& e) {
					return true;
				}
			} else if (key == Server::SERVER)
				*serverBrackets = !*serverBrackets;
			else if (serverKeyMatch(key) && (splited[1].find_first_of(';') == std::string::npos || splited[1].find_last_of(';') != splited[1].find_first_of(';'))) 
				return true;
			splited.at(1)[splited[1].size() - 1] = 0;
			if (key == Server::ERROR)
				error = validateErrorConfig(splited[1]);
			else if (key == Server::HOST)
				error = validateHostConfig(splited[1]);
			else if (key == Server::LIMIT)
				error = validateLimitConfig(splited[1]);
			else if (key == Server::PORT)
				error = validatePortConfig(splited[1]);
			if (error)
				return error;
		}
		line.clear();
		std::getline(file, line);
		trim(line, "\t \n");
	}
	return false;
}

