#include "../includes/Config.hpp"

const char* ServerNotFound::what() const throw()
{
	return ("This server don't exist.");
}

static ServerConfig*	searchViaName(std::string const name, \
	unsigned int const port, \
	std::list< ServerConfig >& servers);

static ServerConfig&	searchViaHost(std::string const& host, \
	unsigned int const port, \
	std::list< ServerConfig >& servers)
throw(ServerNotFound);

ServerConfig&	Config::findByHostNamePort(std::string const& host, \
	std::string const* names, \
	size_t const size, \
	unsigned int const port)
const throw(ServerNotFound)
{
	ServerConfig* foundConfig = NULL;
	if (!host.empty()) {
		return searchViaHost(host, port, \
			const_cast< std::list< ServerConfig >& >(this->_servers));
	} else if (names == NULL) {
		throw ServerNotFound();
	}
	for (size_t i = 0; foundConfig != NULL && i < size; i++) {
		foundConfig = searchViaName(names[i], port, \
			const_cast< std::list< ServerConfig >& >(this->_servers));
	}
	if (foundConfig != NULL) {
		return *foundConfig;
	}
	throw ServerNotFound();
}

void	Config::addServers(std::string const& filename);
bool	Config::configIsValid(std::string const& filename);


static ServerConfig&	searchViaHost(std::string const& host, \
	unsigned int const port, \
	std::list< ServerConfig >& servers)
throw(ServerNotFound)
{
	std::list< ServerConfig >::iterator end(servers.end());
	std::list< ServerConfig >::iterator begin(servers.begin());

	while (begin != end) {
		if (begin->getHost() == host && begin->getPort() == port)
			return *begin;
		++begin;
	}
	throw ServerNotFound();
}


static ServerConfig*	searchViaName(std::string const name, \
	unsigned int const port, \
	std::list< ServerConfig >& servers)
{
	std::list< ServerConfig >::iterator end(servers.end());
	std::list< ServerConfig >::iterator begin(servers.begin());

	while (begin != end) {
		for (size_t i = 0; i < begin->getSizeNames(); i++) {
			if (begin->getNames()[i] == name && begin->getPort() == port)
				return &(*begin);
		}
		++begin;
	}

	return NULL;
}
