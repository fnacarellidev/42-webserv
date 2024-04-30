#pragma once

#include "includeAll.hpp"
#include "HttpStatus.hpp"
#include "RouteConfig.hpp"

#define SERVER_NAME "webserver/1.0"
#define HTTP_VERSION "HTTP/1.1"
#define DEFAULT_LIMIT 1000000
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8080

class ServerConfig {
	public:
		std::map<int, std::string>	errors;
		std::vector<RouteConfig*>	routes;
		std::string root;
		unsigned int	port;
		std::vector<std::string>	serverNames;
		std::string	host;
		size_t	bodyLimit;
		bool	portSetted;

		ServerConfig();
		void	insertError(int code, std::string path);
		RouteConfig*	getRouteByPath(std::string requestUri);
		std::string*	getFilePathFromStatusCode(int status);
};

std::ostream&	operator<<(std::ostream& os, ServerConfig& serverConfig);
