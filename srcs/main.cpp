#include <iostream>
#include <cstdlib>
#include "../includes/Config.hpp"

int main(int argc, char** argv) {
	Config config;

	if (argc != 2) {
		config.servers.push_back(ServerConfig());
		config.servers.back().getRoutes().push_back(RouteConfig());
		std::cout << config << std::endl;
		return 1;
	}
	config.configIsValid(argv[1]);
	config.servers.push_back(ServerConfig());
	// config.addServers(argv[1]);
	config.servers.back().getRoutes().push_back(RouteConfig());
	std::cout << config << std::endl;
	return 0;
}
