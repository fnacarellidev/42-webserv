#pragma once

#include <iostream>
#include <sstream>
#include <utility>
#include <list>

#include "Request.hpp"
#include "RouteConfig.hpp"

class ServerConfig
{
	private:
		// default so 404 q a response vai ser um html <h1>404</h1>
		std::list<std::pair<HttpStatus::Code, std::string>>	_errors;
		std::list< RouteConfig >	_routes;
		// default pra 4200
		unsigned int	_port;
		// default e '_' que e o mesmo q o host
		std::list<std::string>	_serverNames;
		// o literalmente o *_serverNames
		std::string	_defaultName;
		// default vai ser localhost em ip (127.0.0.1)
		std::string	_host;
		// default de 1mb, igual o nginx faz?
		size_t	_bodyLimit;
		size_t	_sizeNames;
	/*
	*	for the routes class
	*	CGI? do a interface? abstract class? normal class?
	*	"Make it work with POST and GET methods."???????
	*	accept uploaded file... only if POST is set? where to save them is another var!
	*/
	public:
		std::pair< HttpStatus::Code, std::string >*	getErrors() { return this->_errors; };
		std::string*	getNames() const { return this->_serverNames; };
		unsigned int getPort() const { return this->_port; };
		std::string	getHost() const { return this->_host; };
		std::string	getDefaultName() const { return this->_defaultName; };
		size_t	getLimit() const { return this->_bodyLimit; };
		size_t	getSizeNames() const { return this->_sizeNames; };
};
