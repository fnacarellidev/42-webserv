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
	if (config.configIsValid(argv[1])) {
		try {
			config.addServers(argv[1]);
		} catch (std::runtime_error& e) {
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}
		std::cout << config << std::endl;
		return EXIT_SUCCESS;
	}
	std::cerr << "Invalid config file." << std::endl;
	return EXIT_FAILURE;
}
