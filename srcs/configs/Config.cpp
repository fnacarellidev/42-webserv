#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"

std::ostream&	operator<<(std::ostream& o, const Config& webserv) {
	size_t i = 0;
	std::vector<ServerConfig> servers = webserv.servers;

	for (std::vector<ServerConfig>::iterator its = servers.begin(); its != servers.end(); its++) {
		o << "server " << i++ << ":" << std::endl;
		o << "\tport: " << its->getPort() << std::endl;
		o << "\tlimit: " << its->getLimit() << std::endl;
		o << "\thost: " << its->getHost() << std::endl;
		o << "\troot: " << its->getServerRoot() << std::endl;

		std::map<int, std::string> erros = its->getErrors();
		std::vector<std::string> nomes = its->getNames();

		for (std::map<int, std::string>::iterator ite = erros.begin(); ite != erros.end(); ite++)
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


ServerConfig&	Config::findByHostNamePort(std::string const& host,
std::string const* names,
size_t const size,
unsigned int const port)
const throw(std::runtime_error) {
	ServerConfig* foundConfig = NULL;
	std::vector<ServerConfig>&	servers = const_cast<std::vector<ServerConfig>&>(this->servers);

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

void	Config::addServers(char* filename) throw (std::runtime_error) {
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

		if (splited[1].find_first_of(";") != std::string::npos)
			splited[1].erase(splited[1].end() - 1);
		if (serverMap[splited[0]] == Server::SERVER)
			this->servers.push_back(ServerConfig());
		else if (serverMap[splited[0]] == Server::HOST)
			this->servers.back().setHost(splited[1]);
		else if (serverMap[splited[0]] == Server::PORT)
			this->servers.back().setPort(std::strtoul(splited[1].c_str(), 0, 10));
		else if (serverMap[splited[0]] == Server::NAMES)
			this->servers.back().setServerNames(split(splited[1], ','));
		else if (serverMap[splited[0]] == Server::LIMIT) {
			this->servers.back().setLimit(std::strtoull(splited[1].c_str(), 0, 10) == 0 ?
			std::numeric_limits<size_t>::max() : std::strtoull(splited[1].c_str(), 0, 10));
		} else if (serverMap[splited[0]] == Server::ERROR) {
			std::vector<std::string> errors = split(splited[1], ',');
			for (std::vector<std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
				std::string::iterator itEqual = std::find(it->begin(), it->end(), '=');
				if (*(itEqual + 1) == '/')
					it->erase(itEqual + 1);
			}
			addErrors(splited[1], this->servers.back());
		}
		else if (serverMap[splited[0]] == Server::ROOT)
			servers.back().setServerRoot(splited[1]);
		else
			addRoutes(file, this->servers.back());
	}
	file.close();
}

bool	Config::configIsValid(char* filename) {
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
