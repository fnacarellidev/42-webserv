#include "../../includes/Request.hpp"

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

