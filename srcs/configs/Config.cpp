#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include <fstream>
#include <cstdlib>

std::ostream&	operator<<(std::ostream& o, const Config& webserv)
{
	size_t i = 0;
	std::vector<ServerConfig> servers = webserv.servers;

	o << "server " << i++ << ":" << std::endl;
	for (std::vector<ServerConfig>::iterator its = servers.begin(); its != servers.end(); its++) {
		o << "\tport: " << its->getPort() << std::endl;
		o << "\tlimit: " << its->getLimit() << std::endl;
		o << "\thost: " << its->getHost() << std::endl;

		TStatusPage erros = its->getErrors();
		std::vector<std::string> nomes = its->getNames();

		for (TStatusPage::iterator ite = erros.begin(); ite != erros.end(); ite++)
			o << "\terror: " << ite->first << " " << ite->second << std::endl;
		for (std::vector<std::string>::iterator itn = nomes.begin(); itn != nomes.end(); itn++)
			o << "\tserver_name: " << *itn << std::endl;

		std::vector<RouteConfig> rotas = its->getRoutes();
		size_t j = 0;

		for (std::vector<RouteConfig>::iterator itr = rotas.begin(); itr != rotas.end(); itr++) {
			o << "\tRoute " << j++ << ":" <<std::endl;
			o << std::boolalpha << "\t\tdir_list: " << itr->getDirList() << std::endl;
			o << "\t\tmethods: " << (itr->getAcceptMethodsBitmask() & GET_OK ? "GET " : "") <<
				(itr->getAcceptMethodsBitmask() & POST_OK ? "POST " : "") <<
				(itr->getAcceptMethodsBitmask() & DELETE_OK ? "DELETE" : "") << std::endl;
			o << "\t\troot: " << itr->getRoot() << std::endl;
			o << "\t\tredir: " << itr->getRedirect().first << " " << itr->getRedirect().second << std::endl;
			
			std::vector<std::string> index = itr->getIndex();

			for (std::vector<std::string>::iterator iti = index.begin(); iti != index.end(); iti++)
				o << "\t\tindex: " << *iti << std::endl;
			o << std::endl;
		}
		o << std::endl;
	}
	return o;
}

const char* ServerNotFound::what() const throw()
{
	return ("This server don't exist.");
}

static HttpStatus::Code	matchStatus(std::string const& status)
{
	int code = std::atoi(status.c_str());

	switch (code / 100) {
		case 2:
			switch (code) {
				case 200:
					return HttpStatus::OK;
			}
		break;
		case 4:
			switch (code) {
				case 403:
					return HttpStatus::FORBIDDEN;
				case 404:
					return HttpStatus::NOTFOUND;
				case 405:
					return HttpStatus::NOTALLOWED;
			}
	}
	return HttpStatus::ERROR;
}

static void	addErrors(std::string const& error, ServerConfig& server)
{
	std::vector<std::string> splited = split(error, ',');

	for (std::vector<std::string>::iterator it = splited.begin(); it != splited.end(); ++it) {
		std::vector<std::string> error = split(*it, '=');

		if (error.size() != 2)
			continue ;
		server.setErrors(std::make_pair(matchStatus(error[0]), error[1]));
	}
}

static void	addMethods(std::string const& methods, RouteConfig& route)
{
	std::vector<std::string> splited = split(methods, ',');
	unsigned short	method = NONE_OK;

	for (std::vector<std::string>::iterator it = splited.begin(); it != splited.end(); ++it) {
		switch (it->size()) {
			case 3:
				method |= GET_OK;
				break;
			case 4:
				method |= POST_OK;
				break;
			case 6:
				method |= DELETE_OK;
		}
	}
	route.setAcceptMethodsBitmask(method);
}

static void	addRedirect(std::string const& redirect, RouteConfig& route)
{
	std::vector<std::string> splited = split(redirect, '=');

	route.setRedirect(std::make_pair(splited[0], splited[1]));
}

ServerConfig&	Config::findByHostNamePort(std::string const& host,
std::string const* names,
size_t const size,
unsigned int const port)
const throw(ServerNotFound)
{
	ServerConfig* foundConfig = NULL;
	std::vector<ServerConfig>&	servers = const_cast<std::vector<ServerConfig>&>(this->servers);

	if (!host.empty())
		return searchViaHost(host, port, servers);
	else if (names == NULL)
		throw ServerNotFound();
	for (size_t i = 0; foundConfig != NULL && i < size; i++)
		foundConfig = searchViaName(names[i], port, servers);
	if (foundConfig != NULL)
		return *foundConfig;
	throw ServerNotFound();
}

void	Config::addServers(const char* filename) throw (std::runtime_error)
{
	std::map<std::string, Server::Keywords> serverMap(buildServerMap());
	std::ifstream	file(filename);
	std::string	line;

	if (!file.is_open()) {
		this->servers.push_back(ServerConfig());
		throw std::runtime_error("File not found.");
	}
	while (!file.eof()) {
		std::getline(file, line);
		trim(line, "\t \n");
		if (line.empty() || line[0] == '}')
			continue ;

		std::vector<std::string> splited = split(line, ' ');

		splited.at(1)[splited[1].size() - 1] = 0;
		if (serverMap[splited[1]] == Server::SERVER)
			this->servers.push_back(ServerConfig());
		else if (serverMap[splited[1]] == Server::HOST)
			this->servers.back().setHost(splited[1]);
		else if (serverMap[splited[1]] == Server::PORT)
			this->servers.back().setPort(std::strtoul(splited[1].c_str(), 0, 10));
		else if (serverMap[splited[1]] == Server::NAMES)
			this->servers.back().setServerNames(split(splited[1], ','));
		else if (serverMap[splited[1]] == Server::LIMIT)
			this->servers.back().setLimit(std::strtoull(splited[1].c_str(), 0, 10));
		else if (serverMap[splited[1]] == Server::ERROR)
			addErrors(splited[1], this->servers.back());
		else
			addRoutes(file, this->servers.back());
		}
	file.close();
}

void	addRoutes(std::ifstream& file, ServerConfig& server)
{
	std::map<std::string, Route::Keywords>	routeMap(buildRouteMap());
	std::string	line("");

	server.setRoutes(RouteConfig());
	while (line != "}") {
		std::getline(file, line);
		trim(line, "\t \n");
		if (line.empty() || line[0] == '}')
			break;

		std::vector<std::string> splited = split(line, ' ');

		splited.at(1)[splited[1].size() - 1] = 0;
		if (routeMap[splited[1]] == Route::INDEX)
			server.getRoutes().back().setIndex(split(splited[1], ','));
		else if (routeMap[splited[1]] == Route::REDIRECT)
			addRedirect(splited[1], server.getRoutes().back());
		else if (routeMap[splited[1]] == Route::ROOT)
			server.getRoutes().back().setRoot(splited[1]);
		else if (routeMap[splited[1]] == Route::METHODS)
			addMethods(splited[1], server.getRoutes().back());
		else if (routeMap[splited[1]] == Route::LISTING)
			server.getRoutes().back().setDirList(splited[1] == "on");
	}
}

bool	Config::configIsValid(const char* filename)
{
	std::map<std::string, Server::Keywords> serverMap(buildServerMap());
	std::ifstream	file(filename);
	std::string line, word;
	bool openBrackets = false;

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
