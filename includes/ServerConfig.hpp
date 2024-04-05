#pragma once

#include <iostream>
#include <sstream>
#include <utility>
#include <list>
#include <vector>

#include "HttpStatus.hpp"
#include "RouteConfig.hpp"

class ServerConfig
{
	private:
		std::pair< HttpStatus::Code, std::string >*	_errors;
		std::vector< RouteConfig >	_routes;
		unsigned int	_port;
		std::string*	_serverNames;
		std::string	_defaultName;
		std::string	_host;
		size_t	_bodyLimit;
		size_t	_sizeNames;
	public:
		ServerConfig();
		std::pair< HttpStatus::Code, std::string >*	getErrors() { return this->_errors; };
		std::string*	getNames() const { return this->_serverNames; };
		unsigned int getPort() const { return this->_port; };
		std::string	getHost() const { return this->_host; };
		std::string	getDefaultName() const { return this->_defaultName; };
		size_t	getLimit() const { return this->_bodyLimit; };
		size_t	getSizeNames() const { return this->_sizeNames; };
		std::vector<RouteConfig> getRoutesConfigs() { return _routes; };
};
