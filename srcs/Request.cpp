#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <sstream>

static std::vector<std::string> getRequestLineParams(std::string request) {
	std::string firstLine;
	std::stringstream strStream(request);

	getline(strStream, firstLine);

	return split(firstLine, ' ');
}

static Methods getMethod(std::string method) {
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	return UNKNOWNMETHOD;
}

static std::string getFilePath(std::vector<ServerConfig> serverConfigs, std::string requestUri) {
	std::vector<RouteConfig> routeConfigs = serverConfigs.front().getRoutes();
	std::string root = routeConfigs.front().getRoot();

	if (root[root.size() - 1] == '/')
		root.erase(root.size() - 1);

	return root + requestUri;
}

Request::Request(std::string request, std::list<ServerConfig> serverConfigs) : _serverConfigs(serverConfigs) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);

	method = getMethod(requestLineParams[METHOD]);
	filePath = getFilePath(_serverConfigs, requestLineParams[REQUESTURI]);
}
