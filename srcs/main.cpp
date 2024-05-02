#include "../includes/includeAll.hpp"
#include "../includes/Request.hpp"
#include "../includes/WebServer.hpp"

extern char **environ;
std::vector<int> gServerFds;
std::vector<struct pollfd> gPollFds;
WebServer gConfig;

#ifndef QUEUE_MAX // maximum conections to one socket
# define QUEUE_MAX std::numeric_limits<unsigned short>::max()
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
	int	opt = 1, flags = 0, serverFd;
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
		flags = fcntl(serverFd, F_GETFL);
		if (flags < 0) {
			perror("fcntl");
			goto error_happen;
		}
		if (fcntl(serverFd, F_SETFL, flags | O_NONBLOCK) < 0) {
			perror("fcntl");
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
	pfd.revents = 0;
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

static void	freeRoutes(std::vector<ServerConfig>::iterator begin, std::vector<ServerConfig>::iterator end) {
	for (std::vector<ServerConfig>::iterator it = begin; it != end; ++it) {
		for (std::vector<RouteConfig*>::iterator it2 = it->routes.begin(); it2 != it->routes.end(); ++it2)
			delete *it2;
	}
}

void	handleSignal(int sig) {
	(void)sig;
	std::cerr << std::endl << "Goodbye" << std::endl;
	closeAll(gPollFds);
	freeRoutes(gConfig.servers.begin(), gConfig.servers.end());
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	if (invalidInputs(argc, argv))
		return EXIT_FAILURE;
	if (argc == 2) {
		try {
			gConfig.setupConfig(argv[1]);
		} catch (std::runtime_error& e) {
			std::cerr << e.what() << std::endl;
			freeRoutes(gConfig.servers.begin(), gConfig.servers.end());
			return EXIT_FAILURE;
		}
	}
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
		WebServer::awaitRequest(gPollFds);
		WebServer::handleRequests(gConfig, gServerFds, gPollFds);
	}
	return 0;
}
