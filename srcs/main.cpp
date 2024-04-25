#include "../includes/includeAll.hpp"
#include "../includes/Request.hpp"
#include "../includes/WebServer.hpp"

extern char **environ;
std::vector<int> gServerFds;
std::vector<struct pollfd> gPollFds;
WebServer gConfig;

#ifndef CONNECTIONS // maximum number of newSockets in the poll
# define CONNECTIONS 1000
#endif

#ifndef QUEUE_MAX // maximum conections to one socket
# define QUEUE_MAX std::numeric_limits<unsigned short>::max()
#endif

#ifndef POLL_TIMEOUT_SEC // time poll will keep in hang until an event is triggered
# define POLL_TIMEOUT_SEC 10 * 1000
#endif

#ifndef BUFFER_SIZE // maximum read for a request
# define BUFFER_SIZE 1048576
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

static void	setupSockets(WebServer& config, std::vector<int>& serverFds) throw(std::runtime_error) {
	int	opt = 1, serverFd;
	struct sockaddr_in sockAddr;

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	for (size_t i = 0; i < config.servers.size(); i++) {
		serverFd = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFd < 0) {
			perror("socket");
			goto error_happen;
		}
		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, 4)) {
			perror("setsockopt");
			goto error_happen;
		}
		sockAddr.sin_port = htons(config.servers[i].port);
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

static void	setupPolls(std::vector<int>& serverFds, std::vector<struct pollfd>& pollFds) {
	struct pollfd pfd;

	pfd.events = POLLIN | POLLOUT;
	for (size_t i = 0; i < serverFds.size(); i++) {
		pfd.fd = serverFds[i];
		pollFds.push_back(pfd);
	}
}

static void	setupSignal(void (*handleSignal)(int)) {
	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);
	signal(SIGQUIT, handleSignal);
}

void	handleSignal(int sig) {
	(void)sig;
	std::cerr << std::endl << "Goodbye" << std::endl;
	closeAll(gPollFds);
	for (std::vector<ServerConfig>::iterator it = gConfig.servers.begin(); it != gConfig.servers.end(); ++it) {
		for (std::vector<RouteConfig*>::iterator it2 = it->routes.begin(); it2 != it->routes.end(); ++it2)
			delete *it2;
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	if (invalidInputs(argc, argv))
		return EXIT_FAILURE;
	if (argc == 2)
		gConfig.setupConfig(argv[1]);
	else {
		gConfig.servers.push_back(ServerConfig());
		gConfig.servers.back().routes.push_back(new RouteConfig());
	}
	try {
		setupSockets(gConfig, gServerFds);
	} catch (std::runtime_error& e) {
		return EXIT_FAILURE;
	}
	setupPolls(gServerFds, gPollFds);
	setupSignal(handleSignal);
	while (true) {
		if (poll(&gPollFds[0], gPollFds.size(), POLL_TIMEOUT_SEC) < 0) {
			perror("poll");
			closeAll(gPollFds);
			return EXIT_FAILURE;
		}
		for (size_t i = 0; i < gPollFds.size(); i++) {
			if (gPollFds[i].revents & POLLIN) {
				if (i < gServerFds.size() && gPollFds[i].fd == gServerFds[i]) {
					int newSocket = accept(gServerFds[i], NULL, NULL);

					if (newSocket < 0) {
						perror("accept");
						continue;
					}
					if (gPollFds.size() < CONNECTIONS)
						gPollFds.push_back((struct pollfd){.fd = newSocket, .events = POLLIN | POLLOUT});
					else {
						Response resFull(HttpStatus::SERVICE_UNAVAILABLE);

						std::cerr << "We are full" << std::endl;
						send(newSocket, resFull.response(), resFull.size(), MSG_CONFIRM);
						close(newSocket);
					}
				}
				else {
					char	*buffer = (char*)std::calloc(BUFFER_SIZE, sizeof(char));
					int	ret = recv(gPollFds[i].fd, buffer, BUFFER_SIZE, 0);

					if (ret < 0) {
						Response resErr(HttpStatus::SERVER_ERR);

						std::cerr << "recv error" << std::endl;
						send(gPollFds[i].fd, resErr.response(), resErr.size(), MSG_CONFIRM);
						close(gPollFds[i].fd);
						gPollFds.erase(gPollFds.begin() + i);
					}
					else {
						buffer[ret] = 0;

						Request req(buffer, gConfig.servers, gPollFds[i].fd);
						Response res = req.runRequest();

						send(gPollFds[i].fd, res.response(), res.size(), MSG_CONFIRM);
						close(gPollFds[i].fd);
						gPollFds.erase(gPollFds.begin() + i);
					}
					std::free(buffer);
				}
			}
		}
	}
	closeAll(gPollFds);
	return 0;
}
