#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <sstream>
#include <cstdlib>

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

	this->_fullRequest = request;
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

static struct stat	pathInfo(const std::string &path) {
	struct stat info;
	stat(path.c_str(), &info);
	return (info);
}

static bool pathExists(const std::string &dir) {
	struct stat statbuff;

	if (stat(dir.c_str(), &statbuff) == 0)
		return (true);
	return (false);
}

static std::string	getDir(const std::string &fullPath) {
	if (fullPath[fullPath.size() - 1] == '/' || fullPath.rfind("/") == std::string::npos)
		return (fullPath);
	return (fullPath.substr(0, fullPath.rfind("/")));
}

static std::string	getContentOfRequest(std::string fullRequest) {
	std::string			line;
	std::string			content;
	std::stringstream	ss(fullRequest);

	while (std::getline(ss, line) && line != "\r") {
		continue ;
	}
	while (std::getline(ss, line)) {
		content += line;
	}
	return (content);
}

Response Request::runPost() {
	std::string directoryPath = getDir(this->filePath);
	if (!pathExists(directoryPath) || !S_ISDIR(pathInfo(directoryPath.c_str()).st_mode))
		return (Response(404));

	if (S_ISDIR(pathInfo(this->filePath.c_str()).st_mode))
		return (Response(409));

	if (pathExists(this->filePath))
		return (Response(200));

	std::string content = getContentOfRequest(this->_fullRequest);

	std::ofstream	newFile(this->filePath.c_str());
	newFile.write(content.c_str(), content.length());
	newFile.close();

	return Response(201);
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

	if (S_ISDIR(statbuf.st_mode)) {
		return Response((*(filePath.end() - 1) != '/' ? 301 :
		(_serverConfigs.front().getRoutes().front().getDirList() ? 200 : 403)), filePath);
	}
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

		case POST:
			return runPost();

		/* case DELETE: */
		/* 	runDelete(); */

		default:
			break;
	}
	return Response(200);
}
