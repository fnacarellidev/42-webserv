#include <algorithm>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../includes/Request.hpp"
#include "../includes/Config.hpp"

using namespace std;

#define INT_MAX numeric_limits<int>::max()
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 4096
#endif

int main(int argc, char **argv) {
	int newSocket;
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr sockAddr = {};
	char buffer[BUFFER_SIZE] = { 0 };
	socklen_t addrLen = sizeof(sockAddr);
	int opt = 1;
	if (argc == 0)
		exit (1);

	if (serverFd < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sa_family = AF_INET;
	unsigned short port = htons(8080);
	memcpy(&sockAddr.sa_data, &port, sizeof(port));
	if (bind(serverFd, &sockAddr, sizeof(sockAddr)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(serverFd, INT_MAX) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while (true) {
		if ((newSocket = accept(serverFd, &sockAddr, &addrLen))
				< 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		int ret = recv(newSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("recv");
			exit(EXIT_FAILURE);
		}
		buffer[ret] = 0;
		stringstream copy(buffer);
		string line;
		Config config;

		config.addServers(argv[1]);

		std::cout << config;

		Request req(buffer, config.servers);
		Response res = req.runRequest();
		
		getline(copy, line);
		std::cout << res.response();
		send(newSocket, res.response(), res.size(), 0);
		close(newSocket);
	}
	close(serverFd);

	return 0;
}
