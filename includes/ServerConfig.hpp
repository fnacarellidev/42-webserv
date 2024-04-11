#pragma once

#include <iostream>
#include <sstream>
#include <utility>
#include <map>

#include "HttpStatus.hpp"
#include "RouteConfig.hpp"

#define SERVER_NAME "webserver/1.0"
#define HTTP_VERSION "HTTP/1.1"
#define DEFAULT_LIMIT 1000000
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8080

class ServerConfig {
	public:
		std::map<int, std::string>	_errors;
		std::vector<RouteConfig>	_routes;
		std::string _root;
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
		ServerConfig();
		~ServerConfig();
		std::map<int, std::string> getErrors() const;
		std::vector<RouteConfig> getRoutes() const;
		std::vector<std::string> getNames() const;
		unsigned int getPort() const;
		std::string	getHost() const;
		size_t	getLimit() const;
		void	setHost(std::string host);
		void	setLimit(size_t limit);
		void	insertError(int code, std::string path);
		void	setErrors(std::map<int, std::string> errors);
		void	setRoutes(RouteConfig routeConfig);
		void	setRoutes(std::vector<RouteConfig> routeConfigs);
		void	setPort(unsigned int port);
		void	setServerNames(std::vector<std::string> serverNames);
		void	setServerNames(std::string serverName);
		void	setServerRoot(std::string serverRoot);
		RouteConfig* getRouteByPath(std::string requestUri);
		std::string	getServerRoot();
		std::string *getFilePathFromStatusCode(int status);
};
