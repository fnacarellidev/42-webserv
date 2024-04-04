#include "../../includes/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	
}

std::vector<std::pair<HttpStatus::Code, std::string> > ServerConfig::getErrors() const {
	return _errors;
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

std::string	ServerConfig::getDefaultName() const {
	return _defaultName;
}

size_t	ServerConfig::getLimit() const {
	return _bodyLimit;
}

size_t	ServerConfig::getSizeNames() const {
	return _sizeNames;
}

void ServerConfig::setErrors(std::vector<std::pair<HttpStatus::Code, std::string> > errors) {
	_errors = errors;
}

void ServerConfig::setErrors(std::pair<HttpStatus::Code, std::string> error) {
	_errors.push_back(error);
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
