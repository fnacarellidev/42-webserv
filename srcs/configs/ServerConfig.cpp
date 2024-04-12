#include "../../includes/ServerConfig.hpp"

ServerConfig::ServerConfig():
root(DEFAULT_ROOT), port(DEFAULT_PORT), host(DEFAULT_HOST), bodyLimit(DEFAULT_LIMIT) {
	serverNames.push_back(host);
}

std::string *ServerConfig::getFilePathFromStatusCode(int status) {
	if (errors.find(status) == errors.end())
		return NULL;
	return &errors[status];
}

void	ServerConfig::insertError(int code, std::string path) {
	errors[code] = path;
}

RouteConfig* ServerConfig::getRouteByPath(std::string requestUri) {
	std::string path(requestUri);
	std::size_t uriLastSlash = requestUri.find_last_of('/');
	bool isPath = requestUri.find('.') == std::string::npos;

	if (!isPath && uriLastSlash != std::string::npos)
		path.erase(uriLastSlash + 1);

	for (std::vector<RouteConfig*>::iterator it = routes.begin(); it != routes.end(); ++it) {
		if (path + "/" == (*it)->path || path == (*it)->path)
			return *it;
	}

	while (path != "/") {
		if (path.find_first_of('/') != std::string::npos)
			path.erase(path.find_last_of('/'));

		for (std::vector<RouteConfig*>::iterator it = routes.begin(); it != routes.end(); ++it) {
			if (path + "/" == (*it)->path || path == (*it)->path)
				return *it;
		}
	}
	return NULL;
}

std::ostream&	operator<<(std::ostream& o, ServerConfig& server) {
	o << "\tServer: " << server.host << ":" << server.port << std::endl;
	o << "\tRoot: " << server.root << std::endl;
	o << "\tBody limit: " << server.bodyLimit << std::endl;
	o << "\tServer names: ";
	for (std::vector<std::string>::iterator it = server.serverNames.begin(); it != server.serverNames.end(); ++it)
		o << *it << " ";
	o << std::endl;
	o << "\tErrors: ";
	for (std::map<int, std::string>::iterator it = server.errors.begin(); it != server.errors.end(); ++it) {
		o << it->first << " " << it->second;
		if (it != --server.errors.end())
			o << ", ";
	}
	o << std::endl;

	size_t n = 1;

	for (std::vector<RouteConfig*>::iterator it = server.routes.begin(); it != server.routes.end(); ++it) {
		o << "\tRoute " << n++ << ": {" << std::endl;
		o << **it << std::endl;
		o << "\t}" << std::endl;
	}
	return o;
}

