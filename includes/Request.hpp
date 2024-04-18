#pragma once
#include "includeAll.hpp"
#include "Response.hpp"
#include "ServerConfig.hpp"
#include "utils.hpp"

enum Methods {
	GET,
	POST,
	DELETE,
	UNKNOWNMETHOD
};

enum RequestLine {
	METHOD,
	REQUESTURI,
	PROTOCOLVER
};

class Response;

class Request {
	private:
		std::string	_fullRequest;
		bool _dirListEnabled;
		HttpStatus::Code runGet();
		HttpStatus::Code runPost();
		HttpStatus::Code runDelete();
	public:
		Methods method;
		bool _shouldRedirect;
		RouteConfig* route;
		std::string	_reqUri;
		std::string _locationHeader;
		std::string filePath;
		std::string	root;
		ServerConfig _server;
		Response runRequest();
		Request(std::string request, std::vector<ServerConfig> serverConfigs);
};
