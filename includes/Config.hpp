#pragma once

#include <list>
#include <exception>
#include <map>
#include "ServerConfig.hpp"

enum ServerKeywords
{
	SERVER = 1,
	HOST,
	PORT,
	NAMES,
	LIMIT,
	ERROR,
	ROUTE
};

enum RouteKeywords
{
	INDEX = 1,
	REDIRECT,
	ROOT,
	METHODS,
	LISTING,
	CGI
};

class ServerNotFound: public std::exception
{
	public:
		virtual const char* what() const throw();
};

class Config
{
	private:
		std::list< ServerConfig >	_servers;
	public:
		ServerConfig&	findByHostNamePort(std::string const& host,\
			std::string const* names, size_t const size, unsigned int const port) \
			const throw(ServerNotFound);
		void	addServers(std::string const& filename);
		bool	configIsValid(std::string const& filename);
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

/*
phind example in yml:
# Configuração para o servidor 1
server {
    host: 127.0.0.1
    port: 8080
    server_names: example.com www.example.com

    # Páginas de erro padrão
    error_pages {
        404: /errors/404.html
        500: /errors/500.html
    }

    # Limite de tamanho do corpo do cliente
    client_body_size_limit: 10MB

    # Rotas
    route {
        path: /api
        methods: [GET, POST]
        directory_listing: off
        default_file: index.html
        cgi_executable: /usr/bin/php-cgi
        cgi_path_info: true
        cgi_directory: /var/www/cgi-bin
    }

    route {
        path: /uploads
        methods: [POST]
        upload_directory: /var/www/uploads
    }
}

# Configuração para o servidor 2
server {
    host: 0.0.0.0
    port: 8888

    # Rotas
    route {
        path: /redirect
        redirect: /new_location
    }

    route {
        path: /files
        file_root: /var/www/static
    }
}
*/
