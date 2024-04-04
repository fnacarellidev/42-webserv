#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

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

static bool pathIsDir(const char *filePath) {
	struct stat statbuf;

	stat(filePath, &statbuf);
	return S_ISDIR(statbuf.st_mode);
}

static std::string getFilePath(std::list<ServerConfig> serverConfigs, std::string requestUri) {
	std::list<RouteConfig> routeConfigs = serverConfigs.front().getRoutesConfigs();
	std::string root = routeConfigs.front().getRoot();

	return root + requestUri;
}

Request::Request(std::string request, std::list<ServerConfig> serverConfigs) : _serverConfigs(serverConfigs) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);

	method = getMethod(requestLineParams[METHOD]);
	filePath = getFilePath(_serverConfigs, requestLineParams[REQUESTURI]);
	isDir = pathIsDir(filePath.c_str());
}
