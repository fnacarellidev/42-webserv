#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <unistd.h>
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

Request::Request(std::string request, std::vector<ServerConfig> serverConfigs) : _serverConfigs(serverConfigs) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);

	method = getMethod(requestLineParams[METHOD]);
	filePath = getFilePath(_serverConfigs, requestLineParams[REQUESTURI]);
}

int    fileGood(const char *filePath) {
	bool fileExists = !access(filePath, F_OK);
	bool canRead = !access(filePath, R_OK);

	if (!fileExists)
		return ENOENT;
	else if (!canRead)
		return EACCES;
	return 0;
}

unsigned short getBitmaskFromMethod(Methods method) {
	switch (method) {
		case GET:
			return GET_OK;
		case POST:
			return POST_OK;
		case DELETE:
			return DELETE_OK;
		case UNKNOWNMETHOD:
			return NONE_OK;
	};
}

Response Request::runRequest() {
	unsigned short methodBitmask = getBitmaskFromMethod(method);

	if (!(methodBitmask & _serverConfigs.front().getRoutes().front().getAcceptMethodsBitmask()))
		return Response(405);
	switch (fileGood(filePath.c_str())) {
		case ENOENT:
			return Response(404);

		case EACCES:
			return Response (403);

		default:
			break;
	}
	return Response(200, filePath);
}
