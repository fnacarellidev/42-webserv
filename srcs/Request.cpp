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

static std::string getFilePath(RouteConfig *route, std::string requestUri) {
	std::string root = route->getRoot();
	std::vector<std::string> indexes = route->getIndex();
	std::string file = requestUri.substr(route->getPath().size() - 1, std::string::npos);

	std::cout << "[DEBUG] START CALL\n\n";

	if (strEndsWith(requestUri, '/')) {
		file.erase(file.end() - 1);
		std::cout << "[DEBUG][GETFILEPATH] SINCE REQURI ENDED WITH SLASH, RETURNING: " << root + file << std::endl;
		std::cout << "\n[DEBUG] END CALL\n";
		return root + file;
	}
	if (file.empty()) {
		for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
			std::cout << "[DEBUG][GETFILEPATH] TESTING ACCESS FOR " << root + *it << std::endl;
			bool fileExists = access((root + *it).c_str(), F_OK) == 0;
			if (fileExists) {
				std::cout << "[DEBUG][GETFILEPATH] FOUND INDEX FOR THIS ROUTE, RETURNING " << root + *it << std::endl;
				std::cout << "\n[DEBUG] END CALL\n";
				return root + *it;
			}
		}
	}
	if (*file.begin() == '/')
		file.erase(file.begin());

	std::cout << "[DEBUG][GETFILEPATH] RETURNING " << root + file << std::endl;
	std::cout << "\n[DEBUG] END CALL\n";

	return root + file;
}

Request::Request(std::string request, std::vector<ServerConfig> serverConfigs) : _serverConfigs(serverConfigs) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);
	std::string requestUri = requestLineParams[REQUESTURI];

	method = getMethod(requestLineParams[METHOD]);
	_reqUri = requestUri;
	_route = _serverConfigs.front().getRouteByPath(requestUri);
	_dirListEnabled = _route->getDirList();
	_route ? filePath = getFilePath(_route, requestUri) : filePath = "";
}

unsigned short getBitmaskFromMethod(Methods method) {
	switch (method) {
		case GET:
			return GET_OK;
		case POST:
			return POST_OK;
		case DELETE:
			return DELETE_OK;
		default:
			return NONE_OK;
	};
}

static bool methodIsAllowed(Methods method, unsigned short allowedMethodsBitmask) {
	unsigned short methodBitmask = getBitmaskFromMethod(method);

	return !(methodBitmask & allowedMethodsBitmask);
}

Response Request::runGet() {
	struct stat statbuf;
	std::string* errPagePath;
	int status = HttpStatus::OK;

	stat(filePath.c_str(), &statbuf);
	switch (fileGood(this->filePath.c_str())) {
		case ENOENT:
			status = HttpStatus::NOTFOUND;
			break;

		case EACCES:
			status = HttpStatus::FORBIDDEN;
			break;

		default:
			break;
	}
	if (status != HttpStatus::OK) {
		errPagePath = _serverConfigs.front().getFilePathFromStatusCode(status);
		return errPagePath ? Response(status, *errPagePath) : Response(status);
	}

	if (S_ISDIR(statbuf.st_mode))
		return Response((_serverConfigs.front().getRoutes().front()->getDirList() ? 200 : 403), filePath);
	if (strEndsWith(_reqUri, '/')) { // example: /webserv/assets/style.css/  it is not a dir, so it wont trigger the condition above.
		if (!_dirListEnabled || access(filePath.c_str(), R_OK) == -1)
			return Response(HttpStatus::FORBIDDEN);
		return Response(HttpStatus::NOTFOUND);
	}
	return Response(200, filePath);
}

Response Request::runRequest() {
	if (!_route) {
		std::cout << "[DEBUG] No match for this route\n";
		return Response(404);
	}

	if (methodIsAllowed(method, _route->getAcceptMethodsBitmask())) {
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
