#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

static Methods getMethod(std::string method) {
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	return UNKNOWNMETHOD;
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

static bool	bodyOverflow(std::string request, size_t const limit) {
	size_t	pos = 0;

	pos = request.find("Content-Length:");
	if (pos == std::string::npos)
		return false;
	request = request.substr(pos, request.find("\r\n", pos) - pos).substr(sizeof("Content-Length:"));
	return (std::strtoull(request.c_str(), NULL, 10) > limit ? true : false);
}

void Request::initRequest(std::string &request) {
	bool transferEncodingChunked = false;
	std::vector<std::string>	requestLineParams = utils::split(request, "\r\n");
	std::vector<std::string>::iterator	it = requestLineParams.begin();

	if (it == requestLineParams.end())
		return ;
	std::vector<std::string>	firstLine = utils::split(*it, ' ');
	this->method = getMethod(firstLine[METHOD]);
	this->_reqUri = firstLine[REQUESTURI];
	it++;
	while (it != requestLineParams.end() && *it != "") {
		if ((*it).find("Host:") != std::string::npos)
			this->_host = (*it).substr(6);
		else if ((*it).find("Content-Length:") != std::string::npos)
			this->_contentLength = std::atoi((*it).substr(16).c_str());
		else if ((*it).find("Content-Type:") != std::string::npos)
			this->_contentType = (*it).substr(14);
		else if ((*it).find("Transfer-Encoding: chunked") != std::string::npos)
			transferEncodingChunked = true;
		it++;
	}
	it++;
	if (transferEncodingChunked) {
		std::vector<std::string>::iterator	itNext = it + 1;
		while (it != requestLineParams.end() && itNext != requestLineParams.end()) {
			if (*it == "0")
				break ;
			this->_contentLength += std::strtol((*it).c_str(), NULL, 16);
			this->_body += *itNext;
			it = itNext + 1;
			itNext = it + 1;
		}
	} else {
		while (it != requestLineParams.end()) {
			this->_body += *it;
			it++;
		}
	}
}

Request::Request(std::string request, std::vector<ServerConfig> serverConfigs, int connectionFd) :
	_host(""),
	_contentType(""),
	_contentLength(0),
	_body(""),
	_fullRequest(request),
	_dirListEnabled(false),
	_connectionFd(connectionFd),
	_shouldRedirect(false),
	execCgi(false)
{
	initRequest(request);
	_server = utils::getServer(serverConfigs, this->_host);
	route = _server.getRouteByPath(this->_reqUri);

	if (route && route->path.size() <= this->_reqUri.size() ) { // localhost:8080/webserv would break with path /webserv/ because of substr below, figure out how to solve.
		if (!route->redirect.first.empty()) {
			_shouldRedirect = this->_reqUri.substr(route->path.size()) == route->redirect.first;
			_locationHeader = "http://localhost:" + utils::toString(_server.port) + route->path + route->redirect.second;
		}
	}
	if (route)
		_dirListEnabled = route->dirList;
	route ? filePath = utils::getFilePath(route, this->_reqUri) : filePath = "";
	execCgi = shouldRunCgi(this->filePath, this->route->cgi);
}

HttpStatus::Code Request::runGet() {
	struct stat statbuf;
	HttpStatus::Code status = HttpStatus::OK;

	stat(filePath.c_str(), &statbuf);
	if (_shouldRedirect) {
		std::string sysFilePath = route->root + route->redirect.second;

		if (stat(sysFilePath.c_str(), &statbuf) == -1)
			return (HttpStatus::NOT_FOUND);
		return (HttpStatus::MOVED_PERMANENTLY);
	}
	switch (utils::fileGood(this->filePath.c_str())) {
		case ENOENT:
			status = HttpStatus::NOT_FOUND;
			break;

		case EACCES:
			status = HttpStatus::FORBIDDEN;
			break;

		default:
			break;
	}
	if (status != HttpStatus::OK) {
		return (status);
	}

	if (S_ISDIR(statbuf.st_mode)) {
		if (route->dirList)
			return (status);
		else
			status = HttpStatus::FORBIDDEN;
		return (status);
	}
	if (utils::strEndsWith(_reqUri, '/')) { // example: /webserv/assets/style.css/  it is not a dir, so it wont trigger the condition above.
		if (!_dirListEnabled || access(filePath.c_str(), R_OK) == -1)
			status = HttpStatus::FORBIDDEN;
		else
			status = HttpStatus::NOT_FOUND;
		return (status);
	}
	if (this->execCgi) {
		try {
			this->cgiOutput = getCgiOutput(this->filePath, this->_connectionFd, "");
		} catch (std::exception &e) {
			return HttpStatus::TIMEOUT;
		}
		this->resContentType = "text/plain";
	}
	return (HttpStatus::OK);
}

HttpStatus::Code Request::runPost() {
	if (this->_contentType == "text/plain") {
		switch (utils::checkPath(this->filePath)) {
			case ENOENT:
				break ;
			case EACCES:
				return (HttpStatus::FORBIDDEN);
			case ENOTDIR:
				this->filePath = "";
				return (HttpStatus::OK);
			default:
				return (HttpStatus::CONFLICT);
		}
		std::string prevPath = utils::getPrevPath(filePath);
		switch (utils::checkPath(prevPath)) {
			case ENOENT:
				return (HttpStatus::NOT_FOUND);
			case EACCES:
				return (HttpStatus::FORBIDDEN);
			case ENOTDIR:
				return (HttpStatus::BAD_REQUEST);
			default:
				break ;
		}
		std::ofstream	file(this->filePath.c_str());
		file.write(this->_body.c_str(), this->_contentLength);
		file.close();
		return (HttpStatus::CREATED);
	}
	if (this->_fullRequest.find("application/x-www-form-urlencoded") != std::string::npos) {
		if (!this->execCgi)
			return HttpStatus::FORBIDDEN;
		try {
			this->cgiOutput = getCgiOutput(this->filePath, this->_connectionFd, this->_body);
		} catch (std::exception &e) {
			return HttpStatus::SERVER_ERR;
		}
		this->resContentType = "text/html";

		return HttpStatus::OK;
	}
	return (HttpStatus::NOT_IMPLEMENTED);
}


HttpStatus::Code	Request::runDelete() {
	struct stat	statbuf;

	if (utils::checkParentFolderPermission(filePath, route->root))
		return (HttpStatus::FORBIDDEN);
	if (access(filePath.c_str(), F_OK))
		return (HttpStatus::NOT_FOUND);
	if (stat(filePath.c_str(), &statbuf)) {
		perror("stat");
		return (HttpStatus::SERVER_ERR);
	}
	if (S_ISDIR(statbuf.st_mode)) {
		if (utils::strEndsWith(_reqUri, '/'))
			return utils::deleteEverythingInsideDir(filePath, this->route->root);
		return (HttpStatus::CONFLICT);
	}
	return utils::tryToDelete(filePath);
}

Response Request::runRequest() {
	if (!route)
		return Response(HttpStatus::NOT_FOUND, *this);

	if (bodyOverflow(this->_fullRequest, this->_server.bodyLimit))
		return Response(HttpStatus::PAYLOAD_TOO_LARGE, *this);
	if (methodIsAllowed(method, route->acceptMethodsBitmask))
		return (Response(HttpStatus::NOT_ALLOWED, *this));
	switch (method) {
		case GET:
			return Response(runGet(), *this);
		case POST:
			return Response(runPost(), *this);
		default:
			return Response(runDelete(), *this);
	}
}
