#include "../../includes/WebServer.hpp"
#include "../../includes/utils.hpp"

static ServerConfig&	searchViaHost(std::string const& host,
unsigned int const port,
std::vector<ServerConfig>& servers)
throw(std::runtime_error) {
	std::vector<ServerConfig>::iterator begin(servers.begin());

	while (begin != servers.end()) {
		if (begin->host == host && begin->port == port)
			return *begin;
		++begin;
	}
	throw std::runtime_error("Server not found");
}

static ServerConfig*	searchViaName(std::string const name,
unsigned int const port,
std::vector<ServerConfig>& servers) {
	std::vector<ServerConfig>::iterator begin(servers.begin());

	while (begin != servers.end()) {
		std::vector<std::string> names = begin->serverNames;

		for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++) {
			if (*it == name && begin->port == port)
				return &(*begin);
		}
		++begin;
	}
	return NULL;
}

ServerConfig&	WebServer::findByHostNamePort(std::string const& host,
std::string const* names,
size_t const size,
unsigned int const port)
throw(std::runtime_error) {
	ServerConfig* foundConfig = NULL;
	std::vector<ServerConfig>&	servers = this->servers;

	if (!host.empty())
		return searchViaHost(host, port, servers);
	else if (names == NULL)
		throw std::runtime_error("Server not found");
	for (size_t i = 0; foundConfig != NULL && i < size; i++)
		foundConfig = searchViaName(names[i], port, servers);
	if (foundConfig != NULL)
		return *foundConfig;
	throw std::runtime_error("Server not found");
}

void	WebServer::setupConfig(char* filename) {
	std::ifstream	file(filename);

	if (!file.is_open()) {
		this->servers.push_back(ServerConfig());
		this->servers.back().routes.push_back(new RouteConfig());
		return ;
	}
	addServers(file, this->servers);
}

bool	WebServer::configIsValid(char* filename) {
	std::map<std::string, Server::Keywords>	serverMap(buildServerMap());
	std::ifstream	file(filename);
	std::string	line, word;
	bool	openBrackets = false;

	if (!file.is_open())
		return false;
	while (!file.eof()) {
		std::getline(file, line);
		trim(line, "\t \n");
		if (line.empty())
			continue ;
		word = line.substr(0, line.find(' '));
		if (word == "}" && !openBrackets)
			goto ret_error;
		if (word == "}") {
			openBrackets = !openBrackets;
			continue ;
		}
		if (serverMap.find(word) == serverMap.end() || serverMap.find(word)->second != Server::SERVER)
			goto ret_error;
		if (invalidServerInputs(file, line, &openBrackets, serverMap))
			goto ret_error;
	}
	file.close();
	return true;
ret_error:
	std::cerr << "bad line: `" << line << "`" << std::endl;
	file.close();
	return false;
}

std::ostream&	operator<<(std::ostream& o, WebServer& webserv) {
	size_t n = 1;
	for (std::vector<ServerConfig>::iterator it = webserv.servers.begin(); it != webserv.servers.end(); it++) {
		o << "Server " << n++ << ": {" << std::endl;
		o << *it;
		o << "}" << std::endl;
	}
	return o;
}
