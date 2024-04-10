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

	return root + requestUri;
}

Request::Request(std::string request, std::vector<ServerConfig> serverConfigs) : _serverConfigs(serverConfigs) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);

	file = requestLineParams[REQUESTURI].erase(0, 1);
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

static bool methodIsAllowed(Methods method, unsigned short allowedMethodsBitmask) {
	unsigned short methodBitmask = getBitmaskFromMethod(method);

	return !(methodBitmask & allowedMethodsBitmask);
}

bool Request::shouldRedirect() {
	std::pair<std::string, std::string> redirect = _serverConfigs.front().getRoutes().front().getRedirect();

	return file == redirect.first;
}

Response Request::runGet() {
	struct stat statbuf;
	std::string* errPagePath;
	int status = HttpStatus::OK;

	std::cout << "[DEBUG] FILEPATH: " << filePath << std::endl;
	stat(filePath.c_str(), &statbuf);
	if (shouldRedirect()) {
		std::cout << "[DEBUG] REDIRECTING...\n";
		std::string routeRoot = _serverConfigs.front().getRoutes().front().getRoot();
		std::string redirectFile = _serverConfigs.front().getRoutes().front().getRedirect().second;
		std::string redirectPath = routeRoot + redirectFile;

		std::cout << redirectPath << std::endl;
		if (stat(redirectPath.c_str(), &statbuf) == -1)
			return Response(HttpStatus::NOTFOUND);
		return Response(HttpStatus::MOVED_PERMANENTLY, redirectPath, "http://localhost:8080/" + redirectFile);
	}
	switch (fileGood(this->filePath.c_str())) {
		case ENOENT:
			std::cout << "Couldn't find " << filePath << std::endl;
			status = HttpStatus::NOTFOUND;
			break;

		case EACCES:
			status = HttpStatus::FORBIDDEN;
			break;

		default:
			break;
	}
	if (status != HttpStatus::OK) {
		std::cout << "[DEBUG] STATUS DIFF OK\n";
		errPagePath = _serverConfigs.front().getFilePathFromStatusCode(status);
		return errPagePath ? Response(status, *errPagePath) : Response(status);
	}

	if (S_ISDIR(statbuf.st_mode))
		return Response((_serverConfigs.front().getRoutes().front().getDirList() ? 200 : 403), filePath);
	return Response(200, filePath);
}

Response Request::runRequest() {
	unsigned short allowedMethodsBitmask = _serverConfigs.front().getRoutes().front().getAcceptMethodsBitmask();

	if (methodIsAllowed(method, allowedMethodsBitmask)) {
		int status = HttpStatus::NOTALLOWED;
		std::string* errPagePath = _serverConfigs.front().getFilePathFromStatusCode(status);
		return errPagePath ? Response(status, *errPagePath) : Response(status);
	}
	switch (method) {
		case GET:
			return runGet();

		/* case POST: */
		/* 	runPost(); */

		/* case DELETE: */
		/* 	runDelete(); */

		default:
			break;
	}
	return Response(200);
}
