#include "../includes/includeAll.hpp"
#include "../includes/Request.hpp"
#include "../includes/WebServer.hpp"

#define OPT 1
#ifndef CONNECTIONS // maximum number of newSockets in the poll
#	define CONNECTIONS 1000
#endif
#ifndef QUEUE_MAX // maximum conections to one socket
#	define QUEUE_MAX std::numeric_limits<short>::max()
#endif
#ifndef POLL_TIMEOUT // time poll will keep in hang until an event is triggered
#	define POLL_TIMEOUT 10 * 1000
#endif
#ifndef BUFFER_SIZE // maximum read for a request
#	define BUFFER_SIZE 4096
#endif

static void	closeAll(std::vector<struct pollfd>& pollFds) {
	for (size_t i = 0; i < pollFds.size(); i++)
		close(pollFds[i].fd);
	pollFds.clear();
}

static bool	invalidInputs(int argc, char **argv) {
	if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << " [config file]" << std::endl;
		return true;
	}
	if (argc == 2 && !WebServer::configIsValid(argv[1]))
		return true;
	return false;
}

// static void	waitPoll(std::vector<struct pollfd>& pollFds) {
// 	if (poll(&pollFds[0], pollFds.size(), POLL_TIMEOUT) < 0) {
// 		perror("poll");
// 		throw std::runtime_error("");
// 	}
// }

static void	setupSockets(WebServer& config, std::vector<int>& serverFds) throw(std::runtime_error) {
	int opt = 1, serverFd;

	for (size_t i = 0; i < config.servers.size(); i++) {
		serverFd = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFd < 0) {
			perror("socket");
			goto error_happen;
		}
		// if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (int*)OPT, 4)) {
		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, 4)) {
			perror("setsockopt");
			goto error_happen;
		}

		struct sockaddr_in sockAddr = {.sin_family = AF_INET, .sin_port = htons(config.servers[i].port)};

		sockAddr.sin_addr.s_addr = INADDR_ANY;
		if (bind(serverFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0) {
			perror("bind");
			goto error_happen;
		}
		if (listen(serverFd, QUEUE_MAX) < 0) {
			perror("listen");
			goto error_happen;
		}
		serverFds.push_back(serverFd);
	}
	return ;
error_happen:
	for (size_t i = 0; i < serverFds.size(); i++)
		close(serverFds[i]);
	std::runtime_error("");
}

int main(int argc, char **argv) {
	std::vector<int> serverFds;
	std::vector<struct pollfd> pollFds;
	WebServer config;

	if (invalidInputs(argc, argv))
		return EXIT_FAILURE;
	if (argc == 2)
		config.setupConfig(argv[1]);
	else {
		config.servers.push_back(ServerConfig());
		config.servers.back().routes.push_back(new RouteConfig());
	}

	try {
		setupSockets(config, serverFds);
	} catch (std::runtime_error& e) {
		return EXIT_FAILURE;
	}

	for (size_t i = 0; i < serverFds.size(); i++)
		pollFds.push_back((struct pollfd){.fd = serverFds[i], .events = POLLIN | POLLOUT});

	while (true) {
		if (poll(&pollFds[0], pollFds.size(), POLL_TIMEOUT) < 0) {
			perror("poll");
			closeAll(pollFds);
			return EXIT_FAILURE;
		}
		for (size_t i = 0; i < pollFds.size(); i++) {
			if (pollFds[i].revents & POLLIN) {
				if (pollFds[i].fd == serverFds[i]) {
					int newSocket = accept(serverFds[i], NULL, NULL);
					
					if (newSocket < 0) {
						perror("accept");
						continue;
					}
					if (pollFds.size() < CONNECTIONS)
						pollFds.push_back((struct pollfd){.fd = newSocket, .events = POLLIN | POLLOUT});
					else {
						std::cerr << "We are full" << std::endl;
						close(newSocket);
					}
				} else {
					char	*buffer = (char*)std::calloc(BUFFER_SIZE, sizeof(char));
					int	ret = recv(pollFds[i].fd, buffer, BUFFER_SIZE, 0);

					if (ret < 0) {
						Response res(500);

						std::cerr << "recv error" << std::endl;
						send(pollFds[i].fd, res.response(), res.size(), MSG_CONFIRM);
						close(pollFds[i].fd);
						pollFds.erase(pollFds.begin() + i);
					} else {
						buffer[ret] = 0;

						Request req(buffer, config.servers);
						Response res = req.runRequest();

						send(pollFds[i].fd, res.response(), res.size(), MSG_CONFIRM);
						close(pollFds[i].fd);
						pollFds.erase(pollFds.begin() + i);
					}
					std::free(buffer);
				}
			}
		}
	}
	closeAll(pollFds);
	return 0;
}

// static void	addResponseSocket(std::vector<struct pollfd>& pollFds, int serverFd) {
// 	int newSocket = accept(serverFd, NULL, NULL);

// 	if (newSocket < 0) {
// 		perror("accept");
// 		return ;
// 	}
// 	if (pollFds.size() < CONNECTIONS) {
// 		pollFds.push_back((struct pollfd){.fd = newSocket, .events = POLLIN | POLLOUT});
// 		return ;
// 	}

// 	Response resFull(HttpStatus::SERVICE_UNAVAILABLE);

// 	std::cerr << "Queue size: " << pollFds.size() << std::endl;
// 	send(newSocket, resFull.response(), resFull.size(), MSG_CONFIRM);
// 	close(newSocket);
// }

// static void	processPoll(std::vector<struct pollfd>& pollFds, std::vector<int>& serverFds, WebServer& config) {
// 	std::string buffer;
// 	for (size_t i = 0; i < pollFds.size(); i++) {
// 		if (pollFds[i].revents & POLLIN) {
// 			if (pollFds[i].fd == serverFds[i])
// 				addResponseSocket(pollFds, serverFds[i]);
// 			else {
// 				char temp[BUFFER_SIZE] = {0};
// 				int ret = recv(pollFds[i].fd, temp, BUFFER_SIZE, 0);

// 				if (ret < 0) {
// 					Response res(HttpStatus::SERVER_ERR);

// 					std::cerr << "recv error" << std::endl;
// 					send(pollFds[i].fd, res.response(), res.size(), MSG_CONFIRM);
// 					close(pollFds[i].fd);
// 					pollFds.erase(pollFds.begin() + i);
// 				} else {
// 					buffer[ret] = 0;

// 					Request req(buffer, config.servers);
// 					Response res = req.runRequest();

// 					send(pollFds[i].fd, res.response(), res.size(), MSG_CONFIRM);
// 					close(pollFds[i].fd);
// 					pollFds.erase(pollFds.begin() + i);
// 				}
// 			}
// 		} else if (pollFds[i].revents & POLLOUT) {
			
// 		}
// 	}
// }
