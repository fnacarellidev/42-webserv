#include <iostream>
#include <cstdlib>
#include "../includes/Config.hpp"

int main(int argc, char* argv[])
{
	Config configuration;

	if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << " config_file" << std::endl;
		return EXIT_FAILURE;
	}
	if (argc == 2 && configuration.configIsValid(argv[1])) {
		configuration.addServers(argv[1]);
		// send to exection
		return EXIT_SUCCESS;
	}
	else if (argc == 1) {
		configuration.servers.push_back(ServerConfig());
		configuration.servers.back().getRoutes().push_back(RouteConfig());
		// send to execution
		return EXIT_SUCCESS;
	}
	std::cerr << "Invalid configuration file, RTFM!" << std::endl;
	return EXIT_FAILURE;
}
