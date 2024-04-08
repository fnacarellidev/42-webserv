#pragma once

#include <iostream>
#include <sstream>
#include <utility>

#include "HttpStatus.hpp"
#include "RouteConfig.hpp"

#define SERVER_NAME "webserver/1.0"
#define HTTP_VERSION "HTTP/1.1"
#define DEFAULT_LIMIT 1000000
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 80

typedef std::vector<std::pair<HttpStatus::Code, std::string> > TStatusPage;

class ServerConfig {
	private:
		TStatusPage	_errors;
		std::vector<RouteConfig>	_routes;
		unsigned int	_port;
		std::vector<std::string>	_serverNames;
		std::string	_host;
		size_t	_bodyLimit;
	/*
	*	for the routes class
	*	CGI? do a interface? abstract class? normal class?
	*	"Make it work with POST and GET methods."???????
	*	accept uploaded file... only if POST is set? where to save them?
	*/
	public:
		ServerConfig();
		TStatusPage getErrors() const;
		std::vector<RouteConfig> getRoutes() const;
		std::vector<std::string> getNames() const;
		unsigned int getPort() const;
		std::string	getHost() const;
		size_t	getLimit() const;
		void	setHost(std::string host);
		void	setLimit(size_t limit);
		void	setErrors(TStatusPage errors);
		void	setErrors(std::pair<HttpStatus::Code, std::string> error);
		void	setRoutes(RouteConfig routeConfig);
		void	setRoutes(std::vector<RouteConfig> routeConfigs);
		void	setPort(unsigned int port);
		void	setServerNames(std::vector<std::string> serverNames);
		void	setServerNames(std::string serverName);
};
