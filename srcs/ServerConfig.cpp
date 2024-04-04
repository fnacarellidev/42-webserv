#include "../includes/ServerConfig.hpp"

std::list<std::pair<HttpStatus::Code, std::string>> ServerConfig::getErrors() {
	return _errors;
}

std::list<RouteConfig> ServerConfig::getRoutes() {
	return _routes;
}

std::list<std::string> ServerConfig::getNames() const {
	return _serverNames;
}

unsigned int ServerConfig::getPort() const {
	return _port;
}

std::string	ServerConfig::getHost() const {
	return _host;
}

std::string	ServerConfig::getDefaultName() {
	return _defaultName;
}

size_t	ServerConfig::getLimit() const {
	return _bodyLimit;
}

size_t	ServerConfig::getSizeNames() const {
	return _sizeNames;
}

void ServerConfig::setErrors(std::list<std::pair<HttpStatus::Code, std::string>> errors) {
	_errors = errors;
}

void ServerConfig::setErrors(std::pair<HttpStatus::Code, std::string> error) {
	_errors.push_back(error);
}

void ServerConfig::setRoutes(std::list<RouteConfig> routeConfigs) {
	_routes = routeConfigs;
}

void ServerConfig::setRoutes(RouteConfig routeConfig) {
	_routes.push_back(routeConfig);
}

void ServerConfig::setPort(unsigned int port) {
	_port = port;
}

void ServerConfig::setServerNames(std::list<std::string> serverNames) {
	_serverNames = serverNames;
}

void ServerConfig::setServerNames(std::string serverName) {
	_serverNames.push_back(serverName);
}
