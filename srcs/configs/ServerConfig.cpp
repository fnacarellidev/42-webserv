#include "../../includes/ServerConfig.hpp"

ServerConfig::ServerConfig() {
	_host = DEFAULT_HOST;
	_port = DEFAULT_PORT;
	_root = DEFAULT_ROOT;
	_bodyLimit = DEFAULT_LIMIT;
	_serverNames.push_back(_host);
	_routes.push_back(RouteConfig());
}

std::map<int, std::string> ServerConfig::getErrors() const {
	return _errors;
}

std::string *ServerConfig::getFilePathFromStatusCode(int status) {
	if (_errors.find(status) == _errors.end())
		return NULL;
	return &_errors[status];
}

std::vector<RouteConfig> ServerConfig::getRoutes() const {
	return _routes;
}

std::vector<std::string> ServerConfig::getNames() const {
	return _serverNames;
}

unsigned int ServerConfig::getPort() const {
	return _port;
}

std::string	ServerConfig::getHost() const {
	return _host;
}

size_t	ServerConfig::getLimit() const {
	return _bodyLimit;
}

void ServerConfig::setErrors(std::map<int, std::string> errors) {
	_errors = errors;
}

void ServerConfig::setRoutes(std::vector<RouteConfig> routeConfigs) {
	_routes = routeConfigs;
}

void ServerConfig::setRoutes(RouteConfig routeConfig) {
	_routes.push_back(routeConfig);
}

void ServerConfig::setPort(unsigned int port) {
	_port = port;
}

void ServerConfig::setServerNames(std::vector<std::string> serverNames) {
	_serverNames = serverNames;
}

void ServerConfig::setServerNames(std::string serverName) {
	_serverNames.push_back(serverName);
}

void	ServerConfig::setHost(std::string host) {
	_host = host;
}

void	ServerConfig::setLimit(size_t limit) {
	_bodyLimit = limit;
}

void	ServerConfig::insertError(int code, std::string path) {
	_errors[code] = path;
}

void	ServerConfig::setServerRoot(std::string serverRoot) {
	_root = serverRoot;
}

std::string	ServerConfig::getServerRoot() {
	return _root;
}
