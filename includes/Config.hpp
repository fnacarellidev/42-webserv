#pragma once

#include "ServerConfig.hpp"

#include <map>
#include <utility>
/*
 * general config
 * all config outside (but also including) `server {}` will be here
*/
class Config
{
	private:
		std::vector< std::pair< std::string, int > >	_serverPorts;
	// 	std::map< std::string, ServerConfig > _configs;
	public:
		std::vector< std::pair< std::string, int > >&	getServerPorts();

	// 	std::map< std::string, ServerConfig >& getConfigs() const;
};

/*
example of conf file:
listen 443;
server_names localrosti.com www2.localrosti.com;
default_server ww2.localrosti.com;
limit_body 10mb;
error 404.html 402.html
# very cool comment
index index.php index.txt;
root /tmp/test42;
dir_listing off;
dir_default ops.php;
server {
	listen 666;
	server_names www.aDeus.com aDeus.com;
	root /tmp/test42/olaDiabo;
	index hello_world.php hello_world.html
	dir_listing on;
	
}
*/
