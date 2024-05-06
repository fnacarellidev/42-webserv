#include "../../includes/WebServer.hpp"
#include "../../includes/utils.hpp"

std::map<std::string, Server::Keywords>	buildServerMap() {
	std::map<std::string, Server::Keywords> map;

	map["server"] = Server::SERVER;
	map["port"] = Server::PORT;
	map["names"] = Server::NAMES;
	map["limit"] = Server::LIMIT;
	map["error"] = Server::ERROR;
	map["route"] = Server::ROUTE;

	return map;
}

std::map<std::string, Route::Keywords>	buildRouteMap() {
	std::map<std::string, Route::Keywords> map;

	map["route"] = Route::ROUTE;
	map["index"] = Route::INDEX;
	map["redirect"] = Route::REDIRECT;
	map["root"] = Route::ROOT;
	map["methods"] = Route::METHODS;
	map["listing"] = Route::LISTING;
	map["path"] = Route::PATH;
	map["cgi"] = Route::CGI;

	return map;
}


