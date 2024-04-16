#include "../../includes/WebServer.hpp"
#include "../../includes/utils.hpp"

static void	addMethods(std::string const& methods, RouteConfig* route) {
	std::vector<std::string>	splited = split(methods, ',');
	unsigned short	method = NONE_OK;

	for (std::vector<std::string>::iterator it = splited.begin(); it != splited.end(); ++it) {
		switch (it->size()) {
			case 3:
				method |= GET_OK;
				break;
			case 4:
				method |= POST_OK;
				break;
			case 6:
				method |= DELETE_OK;
		}
	}
	route->acceptMethodsBitmask = method;
}

static void	addRedirect(std::string const& redirect, RouteConfig* route) {
	std::vector<std::string>	splited = split(redirect, '=');

	if (*splited[0].begin() == '/')
		splited[0].erase(splited[0].begin());
	if (*splited[1].begin() == '/')
		splited[1].erase(splited[1].begin());
	route->redirect = std::make_pair(splited[0], splited[1]);
}

static std::string	removeExtraSlashes(std::string str) {
	std::string	finalStr = "/";
	std::vector<std::string>	splited = split(str, '/');
	std::vector<std::string>::iterator	it;

	for (it = splited.begin(); it != splited.end(); it++)
		if (!(*it).empty())
			finalStr += *it + "/";
	return (finalStr);
}

void	addRoutes(std::ifstream& file, std::string& line, ServerConfig& server) {
	std::map<std::string, Route::Keywords>  routeMap(buildRouteMap());
	std::vector<std::string>	indexes;

	while (line != "}") {
		if (line.empty()) {
			std::getline(file, line);
			trim(line, "\t \n");
			continue ;
		}
		if (line.find_first_of(";") != std::string::npos)
			line.erase(line.end() - 1);

		std::vector<std::string> splited = split(line, ' ');

		switch (routeMap.find(splited[0])->second) {
			case Route::ROUTE:
				server.routes.push_back(new RouteConfig());
				break;
			case Route::INDEX:
				indexes = split(splited[1], ',');

				for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
					if (*it->begin() == '/')
						it->erase(it->begin());
				}
				server.routes.back()->index = indexes;
				break;
			case Route::REDIRECT:
				addRedirect(splited[1], server.routes.back());
				break;
			case Route::ROOT:
				if (*(splited[1].end() - 1) != '/')
					splited[1].insert(splited[1].end(), '/');
				server.routes.back()->root = splited[1];
				break;
			case Route::METHODS:
				addMethods(splited[1], server.routes.back());
				break;
			case Route::LISTING:
				server.routes.back()->dirList = (splited[1] == "on");
				break;
			case Route::PATH:
				server.routes.back()->path = removeExtraSlashes(splited[1]);
				break;
			case Route::CGI:
				server.routes.back()->cgi = split(splited[1], ',');
				break;
		}
		std::getline(file, line);
		trim(line, "\t \n");
	}
}
