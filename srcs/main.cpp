#include "../includes/includeAll.hpp"
#include "../includes/Request.hpp"
#include "../includes/WebServer.hpp"

#define OPT 1
#define CONNECTIONS 1000
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1048576
#endif

static void movePollFd(struct pollfd* pollFds, int i, int serverCount) {
	std::memmove(pollFds + i, pollFds + i + 1, serverCount - i);
}

int main(int argc, char **argv) {
	int serverFds[CONNECTIONS] = {0}, serverCount = 0;
	struct pollfd pollFds[CONNECTIONS] = {};
	WebServer config;

	if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << " [config file]" << std::endl;
		return EXIT_FAILURE;
	}
	if (argc == 1) {
		config.servers.push_back(ServerConfig());
		config.servers.back().routes.push_back(new RouteConfig());
	} else if (argc == 2 && WebServer::configIsValid(argv[1]))
		config.setupConfig(argv[1]);
	else
		return EXIT_FAILURE;
	for (size_t i = 0; i < config.servers.size(); i++) {
		int opt = 1, serverFd = socket(AF_INET, SOCK_STREAM, 0);

		if (serverFd < 0) {
			perror("socket failed");
			return EXIT_FAILURE;
		}
		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
			perror("setsockopt");
			return EXIT_FAILURE;
		}

		struct sockaddr_in sockAddr;

		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(config.servers[i].port);
		sockAddr.sin_addr.s_addr = INADDR_ANY;
		if (bind(serverFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0) {
			perror("bind failed");
			return EXIT_FAILURE;
		}
		if (listen(serverFd, 5) < 0) {
			perror("listen");
			return EXIT_FAILURE;
		}
		serverFds[i] = serverFd;
	}
	for (size_t i = 0; i < config.servers.size(); i++) {
		struct pollfd pollFd = {};

		pollFd.fd = serverFds[i];
		pollFd.events = POLLIN;
		pollFds[i] = pollFd;
	}
	serverCount = config.servers.size();
	while (true) {
			int pollRet = poll(pollFds, serverCount, 10 * 1000);

		if (pollRet < 0) {
			perror("poll");
			return EXIT_FAILURE;
		}
		for (int i = 0; i < serverCount; i++) {
			if (pollFds[i].revents & POLLIN) {
				if (pollFds[i].fd == serverFds[i]) {
					int newSocket = accept(serverFds[i], NULL, NULL);

					if (newSocket < 0) {
						perror("accept");
						continue;
					}
					if (serverCount < CONNECTIONS) {
						struct pollfd pollFd = {};

						pollFd.fd = newSocket;
						pollFd.events = POLLIN;
						pollFds[serverCount] = pollFd;
						serverCount++;
					} else {
						std::cerr << "We are full" << std::endl;
						close(newSocket);
					}
				} else {
					char* buffer = (char*)std::calloc(BUFFER_SIZE, sizeof(char));
					int ret = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);

					if (ret < 0) {
						Response res(500);

						std::cerr << "recv error" << std::endl;
						send(pollFds[i].fd, res.response(), res.size(), MSG_CONFIRM);
						close(pollFds[i].fd);
						movePollFd(pollFds, i, serverCount);
						serverCount--;
					} else {
						buffer[ret] = 0;

						Request req(buffer, config.servers);
						Response res = req.runRequest();

						send(pollFds[i].fd, res.response(), res.size(), MSG_CONFIRM);
						close(pollFds[i].fd);
						movePollFd(pollFds, i, serverCount);
						serverCount--;
					}
					std::free(buffer);
				}
			}
		}
	}
	return 0;
}
