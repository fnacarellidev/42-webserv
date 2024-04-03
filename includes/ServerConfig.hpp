#pragma once

#include <iostream>
#include <sstream>
#include <utility>

class ServerConfig
{
	private:
		std::pair< enum Code, std::string >*	_errors;
		unsigned int	_port;
		std::string*	_serverNames;
		std::string	_host;
		std::string	_defaultName;
		size_t	_bodyLimit;
	/*
	*	for the routes class
	*	std::list< Routes >	_routes;
	*	bool	_dirList;
	*	char	_acceptMethodsBitmask; // 000 = none 001 = get 010 = post 100 = delete 
	*	std::string	_root;
	*	std::pair< std::string, std::string >	_redirect;
	*	std::string*	_index;
	*	CGI? do a interface? abstract class? normal class?
	*	"Make it work with POST and GET methods."???????
	*	accept uploaded file... only if POST is set? where to save them is another var!
	*/
	public:
		std::pair< enum Code, std::string >*	getErrors() { return this->_errors; };
		std::string*	getNames() const { return this->_serverNames; };
		unsigned int getPort() const { return this->_port; };
		std::string	getHost() const { return this->_host; };
		std::string	getDefaultName() const { return this->_defaultName; };
		size_t	getLimit() const { return this->_bodyLimit; };
};
