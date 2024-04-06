#include <iostream>
#include <cstdlib>
#include "../includes/Config.hpp"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: ./webserv config_file" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Hello World!" << std::endl;
	std::cout << "archive validation test" << std::endl;
	Config config;
	config.configIsValid(argv[1]);
	config.servers.push_back(ServerConfig());
	// config.addServers(argv[1]);
	std::cout << config << std::endl;
	config.~Config();
	std::cout << "end test" << std::endl;
	exit(EXIT_SUCCESS);
}
