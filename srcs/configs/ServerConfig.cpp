#include "../../includes/ServerConfig.hpp"

ServerConfig::ServerConfig():
root(DEFAULT_ROOT), port(DEFAULT_PORT), host(DEFAULT_HOST), bodyLimit(DEFAULT_LIMIT), portSetted(false) {
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

	while (path != "/" && !path.empty()) {
		if (path.find_first_of('/') != std::string::npos)
			path.erase(path.find_last_of('/'));

		for (std::vector<RouteConfig*>::iterator it = routes.begin(); it != routes.end(); ++it) {
			if (path + "/" == (*it)->path || path == (*it)->path)
				return *it;
		}
	}
	return NULL;
}
