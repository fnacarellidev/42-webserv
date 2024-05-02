#include "../includes/includeAll.hpp"
#include "../includes/Request.hpp"
#include "../includes/WebServer.hpp"

extern char **environ;
std::vector<int> gServerFds;
std::vector<struct pollfd> gPollFds;
WebServer gConfig;

static void	closeAll(std::vector<struct pollfd>& pollFds);
static void	handleSignal(int sig);
static void	freeRoutes(std::vector<ServerConfig>::iterator begin, std::vector<ServerConfig>::iterator end);
static void	closeAll(std::vector<struct pollfd>& pollFds);
static void	setupSignal(void (*handleSignal)(int));
static bool	invalidInputs(int argc, char **argv);

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
		utils::setupSockets(gConfig, gServerFds);
	} catch (std::runtime_error& e) {
		return EXIT_FAILURE;
	}
	utils::setupPolls(gServerFds, gPollFds);
	setupSignal(handleSignal);
	while (true) {
		WebServer::awaitRequest(gPollFds);
		WebServer::handleRequests(gConfig, gServerFds, gPollFds);
	}
	return 0;
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

static void	closeAll(std::vector<struct pollfd>& pollFds) {
	for (size_t i = 0; i < pollFds.size(); i++)
		close(pollFds[i].fd);
	pollFds.clear();
}

static void	setupSignal(void (*handleSignal)(int)) {
	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);
	signal(SIGQUIT, handleSignal);
	signal(SIGPIPE, SIG_IGN);
}

static void	freeRoutes(std::vector<ServerConfig>::iterator begin, std::vector<ServerConfig>::iterator end) {
	for (std::vector<ServerConfig>::iterator it = begin; it != end; ++it) {
		for (std::vector<RouteConfig*>::iterator it2 = it->routes.begin(); it2 != it->routes.end(); ++it2)
			delete *it2;
	}
}

void	handleSignal(int sig) {
	(void) sig;
	std::cerr << "\nGoodbye\n";
	closeAll(gPollFds);
	freeRoutes(gConfig.servers.begin(), gConfig.servers.end());
	exit(EXIT_SUCCESS);
}
