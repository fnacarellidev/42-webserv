#include "../../includes/WebServer.hpp"
#include "../../includes/utils.hpp"
#include "../../includes/HttpStatus.hpp"

static void	addErrors(std::string const& error, ServerConfig& server) {
	std::vector<std::string> splited = split(error, ',');

	for (std::vector<std::string>::iterator it = splited.begin(); it != splited.end(); ++it) {
		std::vector<std::string> error = split(*it, '=');
		int code = std::atoi(error[0].c_str());

		if (error[1][0] == '/')
			error[1].erase(0, 1);
		server.insertError(code, server.root + error[1]);
	}
}

void	addServers(std::ifstream& file, std::vector<ServerConfig>& servers) {
	std::string	line;
	std::map<std::string, Server::Keywords>	serverMap(buildServerMap());

	while (!file.eof()) {
		std::getline(file, line);
		trim(line, "\t \n");
		if (line.empty())
			continue;
		if (line == "}") {
				if (servers.back().routes.size() == 0)
					servers.back().routes.push_back(new RouteConfig());
				continue ;
		}

		std::vector<std::string> splited = split(line, ' ');
		size_t	limit;

		if (splited[1].find_first_of(";") != std::string::npos)
			splited[1].erase(splited[1].end() - 1);
		switch (serverMap.find(splited[0])->second) {
			case Server::SERVER:
				servers.push_back(ServerConfig());
				break;
			case Server::HOST:
				servers.back().host = splited[1];
				break;
			case Server::PORT:
				servers.back().port = std::strtoul(splited[1].c_str(), NULL, 10);
				break;
			case Server::NAMES:
				servers.back().serverNames = split(splited[1], ',');
				break;
			case Server::LIMIT:
				limit = std::strtoull(splited[1].c_str(), NULL, 10);
				if (limit == 0)
					limit = std::numeric_limits<size_t>::max();
				servers.back().bodyLimit = limit;
				break;
			case Server::ERROR:
				addErrors(splited[1], servers.back());
				break;
			case Server::ROOT:
				if (*(splited[1].end() - 1) != '/')
					splited[1].insert(splited[1].end(), '/');
				servers.back().root = splited[1];
				break;
			case Server::ROUTE:
				addRoutes(file, line, servers.back());
		}
	}
	file.close();
}

