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

int    fileGood(const char *filePath) {
	bool fileExists = !access(filePath, F_OK);
	bool canRead = !access(filePath, R_OK);

	if (!fileExists)
		return ENOENT;
	else if (!canRead)
		return EACCES;
	return 0;
}

static std::string getFilePath(std::vector<ServerConfig> serverConfigs, std::string requestUri) {
	std::vector<RouteConfig> routeConfigs = serverConfigs.front().getRoutes();
	std::string root = routeConfigs.front().getRoot();
	std::vector<std::string> indexes = routeConfigs.front().getIndex();

	if (requestUri == "/") {
		for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
			bool fileExists = access((root + *it).c_str(), F_OK) == 0;
			if (fileExists)
				return root + *it;
		}
	}
	if (root[root.size() - 1] == '/')
		root.erase(root.size() - 1);

	return root + requestUri;
}

Request::Request(std::string request, std::vector<ServerConfig> serverConfigs) : _serverConfigs(serverConfigs) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);

	method = getMethod(requestLineParams[METHOD]);
	filePath = getFilePath(_serverConfigs, requestLineParams[REQUESTURI]);
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
	int status = 200;
	bool failed = false;
	struct stat statbuf;
	unsigned short methodBitmask = getBitmaskFromMethod(method);
	stat(filePath.c_str(), &statbuf);

	if (!(methodBitmask & _serverConfigs.front().getRoutes().front().getAcceptMethodsBitmask())) {
		std::string* errPath = _serverConfigs.front().getFilePathFromStatusCode(405);
		if (errPath)
			return Response(405, *errPath);
		return Response(405);
	}
	switch (fileGood(filePath.c_str())) {
		case ENOENT:
			failed = true;
			status = 404;
			break;

		case EACCES:
			status = 403;
			failed = true;
			break;

		default:
			break;
	}
	if (failed) {
		std::string* errPath = _serverConfigs.front().getFilePathFromStatusCode(status);
		if (errPath)
			return Response(status, *errPath);
		return Response(status);
	}

	if (S_ISDIR(statbuf.st_mode)) {
		std::string* errPath = _serverConfigs.front().getFilePathFromStatusCode(500);
		if (errPath)
			return Response(status, *errPath);
		return Response(500);
	}
	return Response(200, filePath);
}
