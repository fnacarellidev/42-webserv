#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

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

static std::string	getBodyOfRequest(std::string fullRequest) {
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

static std::string	getPrevPath(const std::string &fullPath) {
	if (fullPath[fullPath.size() - 1] == '/' || fullPath.rfind("/") == std::string::npos)
		return (fullPath);
	return (fullPath.substr(0, fullPath.rfind("/")));
}

static int	checkPath(std::string &path) {
	if (!pathExists(path))
		return (ENOENT);
	if (access(path.c_str(), R_OK) != 0)
		return (EACCES);
	if (!S_ISDIR(pathInfo(path.c_str()).st_mode))
		return (ENOTDIR);
	return (0);
}

Response	getResponsePage(int status, std::vector<ServerConfig> &serverConfigs) {
	std::string *errPagePath;
	errPagePath = serverConfigs.front().getFilePathFromStatusCode(status);
	return errPagePath ? Response(status, *errPagePath) : Response(status);
}

Response Request::runPost() {
	switch (checkPath(this->filePath)) {
		case ENOENT:
			break ;
		case EACCES:
			return (getResponsePage(HttpStatus::FORBIDDEN, _serverConfigs));
		case ENOTDIR:
			return (Response(200));
		default:
			return (getResponsePage(409, _serverConfigs));
	}
	std::string prevPath = getPrevPath(filePath);
	switch (checkPath(prevPath)) {
		case ENOENT:
			return (getResponsePage(HttpStatus::NOTFOUND, _serverConfigs));
		case EACCES:
			return (getResponsePage(HttpStatus::FORBIDDEN, _serverConfigs));
		case ENOTDIR:
			return (getResponsePage(400, _serverConfigs));
		default:
			break ;
	}
	std::string		content = getBodyOfRequest(this->_fullRequest);
	std::ofstream	file(this->filePath.c_str());
	file.write(content.c_str(), content.length());
	file.close();
	return Response(201);
}

Response Request::runGet() {
	switch (checkPath(this->filePath)) {
		case ENOENT:
			return (getResponsePage(HttpStatus::NOTFOUND, _serverConfigs));

		case EACCES:
			return (getResponsePage(HttpStatus::FORBIDDEN, _serverConfigs));

		default:
			break ;
	}

	if (S_ISDIR(pathInfo(this->filePath).st_mode))
		return Response((_serverConfigs.front().getRoutes().front().getDirList() ? 200 : 403), filePath);
	return Response(200, this->filePath);
}

Response Request::runRequest() {
	unsigned short allowedMethodsBitmask = _serverConfigs.front().getRoutes().front().getAcceptMethodsBitmask();

	if (methodIsAllowed(method, allowedMethodsBitmask)) {
		return (getResponsePage(HttpStatus::NOTALLOWED, _serverConfigs));
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
