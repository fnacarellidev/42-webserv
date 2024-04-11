#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include "../../includes/HttpStatus.hpp"

void	addErrors(std::string const& error, ServerConfig& server) {
	std::vector<std::string> splited = split(error, ',');

	for (std::vector<std::string>::iterator it = splited.begin(); it != splited.end(); ++it) {
		std::vector<std::string> error = split(*it, '=');
		int code = std::atoi(error[0].c_str());
		if (error[1][0] == '/')
			error[1].erase(0, 1);

		server.insertError(code, server.getServerRoot() + error[1]);
	}
}

void	addMethods(std::string const& methods, RouteConfig& route) {
	std::vector<std::string> splited = split(methods, ',');
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
	route.setAcceptMethodsBitmask(method);
}

void	addRedirect(std::string const& redirect, RouteConfig& route) {
	std::vector<std::string> splited = split(redirect, '=');

	if (*splited[0].begin() == '/')
		splited[0].erase(splited[0].begin());
	if (*splited[1].begin() == '/')
		splited[1].erase(splited[1].begin());
	route.setRedirect(std::make_pair(splited[0], splited[1]));
}

void	addRoutes(std::ifstream& file, ServerConfig& server) {
	std::map<std::string, Route::Keywords>	routeMap(buildRouteMap());
	std::string	line("");
	std::vector<RouteConfig>	routes = server.getRoutes();

	routes.push_back(RouteConfig());
	while (line != "}") {
		std::getline(file, line);
		trim(line, "\t \n");
		if (line.empty() || line[0] == '}')
			break;
		if (line.find_first_of(";") != std::string::npos)
			line.erase(line.end() - 1);

		std::vector<std::string> splited = split(line, ' ');

		if (routeMap[splited[0]] == Route::ROUTE)
			server.setRoutes(RouteConfig());
		else if (routeMap[splited[0]] == Route::INDEX) {
			std::vector<std::string> indexes = split(splited[1], ',');

			for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
				if (*it->begin() == '/')
					it->erase(it->begin());
			}
			routes.back().setIndex(indexes);
		} else if (routeMap[splited[0]] == Route::REDIRECT)
			addRedirect(splited[1], routes.back());
		else if (routeMap[splited[0]] == Route::ROOT) {
			if (*(splited[1].end() - 1) != '/')
				splited[1].insert(splited[1].end(), '/');
			routes.back().setRoot(splited[1]);
		} else if (routeMap[splited[0]] == Route::METHODS)
			addMethods(splited[1], routes.back());
		else if (routeMap[splited[0]] == Route::LISTING)
			routes.back().setDirList(splited[1] == "on");
		else if (routeMap[splited[0]] == Route::PATH)
			routes.back()._path = (splited[1][0] == '/' ? splited[1].erase(0, 1) : splited[1]); 
	}
}
