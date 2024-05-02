#include "../../includes/Request.hpp"

#ifndef QUEUE_MAX // maximum conections to one socket
# define QUEUE_MAX std::numeric_limits<unsigned short>::max()
#endif

static unsigned int	getRequestPort(std::string request) {
	size_t	pos = 0;

	pos = request.find(":");
	if (pos == std::string::npos)
		return (DEFAULT_PORT);
	else
		pos += 1;
	return std::atoi(request.substr(pos, request.find("\r\n", pos) - pos).c_str());
}

std::string utils::getFilePath(RouteConfig *route, std::string requestUri) {
	std::string root = route->root;
	std::vector<std::string> indexes = route->index;
	std::string file = requestUri.substr(route->path.size() - 1, std::string::npos);

	if (utils::strEndsWith(requestUri, '/')) {
		file.erase(file.end() - 1);
		if (*file.begin() == '/')
			file.erase(file.begin());
		return root + file;
	}
	if (file.empty()) {
		for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
			bool fileExists = access((root + *it).c_str(), F_OK) == 0;
			if (fileExists)
				return root + *it;
		}
	}
	if (*file.begin() == '/')
		file.erase(file.begin());

	return root + file;
}

std::string utils::getHeader(std::string request, std::string header) {
	std::string line;
	std::string value;
	size_t		headerLen = header.size();
	std::stringstream sstream(request);

	while (getline(sstream, line)) {
		if (line.substr(0, headerLen) == header) {
			value = line.substr(headerLen);
			utils::trim(value, "\r");
			return value;
		}
	}
	return "";
}

ServerConfig utils::getServer(std::vector<ServerConfig> serverConfigs, std::string host) {
	unsigned int	port = getRequestPort(host);
	std::vector<std::string>::iterator namesIt;
	std::vector<ServerConfig>::iterator it;

	host = host.substr(0, host.find(':'));
	if (host == "localhost" || host == "0.0.0.0")
		host = DEFAULT_HOST;
	for (it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
		std::vector<std::string> names = it->serverNames;
		for (namesIt = names.begin(); namesIt != names.end(); ++namesIt) {
			if (host == *namesIt && it->port == port)
				return *it;
		}
	}
	for (it = serverConfigs.begin(); it != serverConfigs.end(); it++)
		if (it->port == port)
			return *it;
	return serverConfigs.front();
}


void	utils::setupSockets(WebServer& config, std::vector<int>& serverFds) throw(std::runtime_error) {
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

void	utils::setupPolls(std::vector<int>& serverFds, std::vector<struct pollfd>& pollFds) {
	struct pollfd pfd;

	pfd.events = POLLIN | POLLOUT;
	pfd.revents = 0;
	for (size_t i = 0; i < serverFds.size(); i++) {
		pfd.fd = serverFds[i];
		pollFds.push_back(pfd);
	}
}

