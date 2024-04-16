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

static std::string getFilePath(RouteConfig *route, std::string requestUri) {
	std::string root = route->root;
	std::vector<std::string> indexes = route->index;
	std::string file = requestUri.substr(route->path.size() - 1, std::string::npos);

	if (strEndsWith(requestUri, '/')) {
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

std::string getHostHeader(std::string request) {
	std::string line;
	std::string host;
	std::stringstream sstream(request);

	while (getline(sstream, line)) {
		if (line.substr(0, 6) == "Host: ") {
			host = line.substr(6);
			trim(host, "\r");
			return host;
		}
	}
	return "";
}

ServerConfig getServer(std::vector<ServerConfig> serverConfigs, std::string host) {
	for (std::vector<ServerConfig>::iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
		std::vector<std::string> names = it->serverNames;
		for (std::vector<std::string>::iterator namesIt = names.begin(); namesIt != names.end(); ++namesIt) {
			if (host == *namesIt)
				return *it;
		}
	}
	return serverConfigs.front();
}

Request::Request(std::string request, std::vector<ServerConfig> serverConfigs) : _shouldRedirect(false) {
	std::string host = getHostHeader(request);
	std::vector<std::string> requestLineParams = getRequestLineParams(request);
	std::string requestUri = requestLineParams[REQUESTURI];

	_fullRequest = request;
	_reqUri = requestUri;
	_server = getServer(serverConfigs, host);
	method = getMethod(requestLineParams[METHOD]);
	_route = _server.getRouteByPath(requestUri);
	_dirListEnabled = false;
	_shouldRedirect = false;

	if (_route && _route->path.size() <= requestUri.size() ) { // localhost:8080/webserv would break with path /webserv/ because of substr below, figure out how to solve.
		_dirListEnabled = _route->dirList;
		if (!_route->redirect.first.empty())
			_shouldRedirect = requestUri.substr(_route->path.size()) == _route->redirect.first;
	}
	if (_route)
		_dirListEnabled = _route->dirList;
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

static std::string	getBodyOfRequest(std::string fullRequest) {
	std::string			line;
	std::string			content;
	std::stringstream	ss(fullRequest);

	while (std::getline(ss, line) && line != "\r") ;
	while (std::getline(ss, line)) {
		content += line;
	}
	return (content);
}

static Response	getResponsePage(int status, ServerConfig &server) {
	std::string *errPagePath;
	errPagePath = server.getFilePathFromStatusCode(status);
	return errPagePath ? Response(status, *errPagePath) : Response(status);
}

Response Request::runPost() {
	if (this->_fullRequest.find("application/x-www-form-urlencoded") != std::string::npos)
		return (Response(501));
	if (this->_fullRequest.find("text/plain") != std::string::npos) {
		switch (checkPath(this->filePath)) {
			case ENOENT:
				break ;
			case EACCES:
				return (getResponsePage(HttpStatus::FORBIDDEN, _server));
			case ENOTDIR:
				return (Response(200));
			default:
				return (getResponsePage(409, _server));
		}
		std::string prevPath = getPrevPath(filePath);
		switch (checkPath(prevPath)) {
			case ENOENT:
				return (getResponsePage(HttpStatus::NOTFOUND, _server));
			case EACCES:
				return (getResponsePage(HttpStatus::FORBIDDEN, _server));
			case ENOTDIR:
				return (getResponsePage(400, _server));
			default:
				break ;
		}
		std::string		content = getBodyOfRequest(this->_fullRequest);
		std::ofstream	file(this->filePath.c_str());
		file.write(content.c_str(), content.length());
		file.close();
		return Response(201);
	}
	return (Response(501));
}

Response Request::runGet() {
	struct stat statbuf;
	std::string* errPagePath;
	int status = HttpStatus::OK;

	stat(filePath.c_str(), &statbuf);
	if (_shouldRedirect) {
		std::string redirectFile = _route->redirect.second;
		std::string sysFilePath = _route->root + _route->redirect.second;
		std::string requestUrl = "http://localhost:" + toString(_server.port) + _route->path + redirectFile;

		if (stat(sysFilePath.c_str(), &statbuf) == -1)
			return Response(HttpStatus::NOTFOUND);
		return Response(HttpStatus::MOVED_PERMANENTLY, sysFilePath, requestUrl);
	}
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
		errPagePath = _server.getFilePathFromStatusCode(status);
		return errPagePath ? Response(status, *errPagePath) : Response(status);
	}

	if (S_ISDIR(statbuf.st_mode)) {
		if (_route->dirList)
			return Response(status, filePath);
		else
			status = HttpStatus::FORBIDDEN;
		errPagePath = _server.getFilePathFromStatusCode(status);
		return errPagePath ? Response(status, *errPagePath) : Response(status);
	}
	if (strEndsWith(_reqUri, '/')) { // example: /webserv/assets/style.css/  it is not a dir, so it wont trigger the condition above.
		errPagePath = _server.getFilePathFromStatusCode(status);
		if (!_dirListEnabled || access(filePath.c_str(), R_OK) == -1)
			status = HttpStatus::FORBIDDEN;
		else
			status = HttpStatus::NOTFOUND;
		return errPagePath ? Response(status, *errPagePath) : Response(status);
	}
	return Response(200, filePath);
}

Response Request::runRequest() {
	if (!_route)
		return Response(404);

	if (methodIsAllowed(method, _route->acceptMethodsBitmask)) {
		return (getResponsePage(HttpStatus::NOTALLOWED, _server));
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
