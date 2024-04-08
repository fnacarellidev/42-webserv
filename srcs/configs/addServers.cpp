#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include "../../includes/HttpStatus.hpp"

static HttpStatus::Code	matchStatus(std::string const& status) {
	int code = std::atoi(status.c_str());

	switch (code / 100) {
		case 2:
			switch (code) {
				case 200:
					return HttpStatus::OK;
			}
		break;
		case 4:
			switch (code) {
				case 403:
					return HttpStatus::FORBIDDEN;
				case 404:
					return HttpStatus::NOTFOUND;
				case 405:
					return HttpStatus::NOTALLOWED;
			}
	}
	return HttpStatus::NOTFOUND;
}

void	addErrors(std::string const& error, ServerConfig& server) {
	std::vector<std::string> splited = split(error, ',');

	for (std::vector<std::string>::iterator it = splited.begin(); it != splited.end(); ++it) {
		std::vector<std::string> error = split(*it, '=');

		server.setErrors(std::make_pair(matchStatus(error[0]), error[1]));
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

	route.setRedirect(std::make_pair(splited[0], splited[1]));
}

void	addRoutes(std::ifstream& file, ServerConfig& server) {
	std::map<std::string, Route::Keywords>	routeMap(buildRouteMap());
	std::string	line("");
	std::vector<RouteConfig>	routes = server.getRoutes();

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
		else if (routeMap[splited[0]] == Route::INDEX)
			routes.back().setIndex(split(splited[1], ','));
		else if (routeMap[splited[0]] == Route::REDIRECT)
			addRedirect(splited[1], routes.back());
		else if (routeMap[splited[0]] == Route::ROOT)
			routes.back().setRoot(splited[1]);
		else if (routeMap[splited[0]] == Route::METHODS)
			addMethods(splited[1], routes.back());
		else if (routeMap[splited[0]] == Route::LISTING)
			routes.back().setDirList(splited[1] == "on");
	}
	server.getRoutes().erase(server.getRoutes().end() - 1);
	server.setRoutes(routes);
}
