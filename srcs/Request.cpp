#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

char* strdup(std::string str) {
	char* dup = (char *) std::calloc(str.size(), 1);

	for (size_t i = 0; i < str.size(); ++i)
		dup[i] = str[i];

	return dup;
}

static bool shouldRunCgi(std::string filePath, std::vector<std::string> allowedCgis) {
	std::string fileExtension = filePath.substr(filePath.find_last_of('.'));

	for (size_t i = 0; i < allowedCgis.size(); ++i) {
		if (fileExtension == allowedCgis[i])
			return true;
	}

	return false;
}

char **getExecveArgs(std::string filePath, std::string fileExtension, std::string cgiParameter) {
	char **execveArgs;
	if (!cgiParameter.empty()) {
		execveArgs = (char **) std::calloc(4, sizeof(char *));
		execveArgs[2] = ::strdup(cgiParameter.c_str());
	}
	else
		execveArgs = (char **) std::calloc(3, sizeof(char *));

	if (fileExtension == ".py")
		execveArgs[0] = ::strdup("python3");
	else
		execveArgs[0] = ::strdup("php");
	execveArgs[1] = ::strdup(filePath.c_str());

	return execveArgs;
}

void runCgi(std::string filePath, int tmpFileFd, std::string cgiParameter) {
	std::string binPath;
	std::string fileExtension = filePath.substr(filePath.find_last_of('.'));

	if (fileExtension == ".py")
		binPath = "/usr/bin/python3";
	else
		binPath = "/usr/bin/php";

	int pid = fork();

	if (pid == -1) {
		perror("fork");
		throw std::runtime_error("fork");
	}
	else if (pid == 0) {
		char **args = getExecveArgs(filePath, fileExtension, cgiParameter);

		dup2(tmpFileFd, STDOUT_FILENO);
		dup2(tmpFileFd, STDERR_FILENO);
		if (execve(binPath.c_str(), args, environ) == -1) {
			perror("execve");
			throw std::runtime_error("execve");
		}
	}
	else {
		waitpid(pid, NULL, 0);
		close(tmpFileFd);
	}
}

std::string getCgiOutput(std::string filePath, int connectionFd, std::string cgiParameter) {
	std::string cgiOutput;
	std::string tmpFile(".response" + toString(connectionFd));
	int tmpFileFd = open(tmpFile.c_str(), O_CREAT | O_RDWR, 0644);

	if (tmpFileFd == -1) {
		perror("open");
		throw std::runtime_error("open");
	}

	runCgi(filePath, tmpFileFd, cgiParameter);
	cgiOutput = getFileContent(tmpFile);
	std::remove(tmpFile.c_str());

	return cgiOutput;
}

static unsigned int	getRequestPort(std::string request) {
	size_t	pos = 0;

	pos = request.find(":") + 1;
	if (pos == std::string::npos)
		return (DEFAULT_PORT);
	return std::atoi(request.substr(pos, request.find("\r\n", pos) - pos).c_str());
}

static bool	bodyOverflow(std::string request, size_t const limit) {
	size_t	pos = 0;

	pos = request.find("Content-Length:");
	if (pos == std::string::npos)
		return false;
	request = request.substr(pos, request.find("\r\n", pos) - pos).substr(sizeof("Content-Length:"));
	return (std::strtoull(request.c_str(), NULL, 10) > limit ? true : false);
}

static bool checkParentFolderPermission(std::string filePath, const std::string& root) {
	size_t fileSlashes = std::count(filePath.begin(), filePath.end(), '/');
	size_t rootSlashes = std::count(root.begin(), root.end(), '/');

	while (fileSlashes-- > rootSlashes) {
		filePath = getPrevPath(filePath);
		if (access(filePath.c_str(), R_OK | W_OK | X_OK) == -1) {
			perror("access");
			return true;
		}
	}
	return false;
}

static HttpStatus::Code	tryToDelete(const std::string& filePath) {
	if (std::remove(filePath.c_str())) {
		perror("std::remove");
		return (HttpStatus::SERVER_ERR);
	}
	return (HttpStatus::NO_CONTENT);
}

static HttpStatus::Code	deleteEverythingInsideDir(std::string dirPath, std::string& root) {
	HttpStatus::Code	status = HttpStatus::NO_CONTENT;
	DIR	*dir = opendir(dirPath.c_str());

	if (dir == NULL) {
		perror("opendir");
		return (HttpStatus::SERVER_ERR);
	}
	dirPath += '/';
	for (struct dirent *item = readdir(dir); item != NULL; item = readdir(dir)) {
		std::string filePath;

		if (std::strcmp(item->d_name, ".") == 0 || std::strcmp(item->d_name, "..") == 0)
			continue;
		filePath = dirPath + item->d_name;
		if (std::remove(filePath.c_str())) {
			perror("std::remove");
			status = (errno == EACCES ? HttpStatus::FORBIDDEN : HttpStatus::SERVER_ERR);
		}
	}
	closedir(dir);
	dirPath.erase(0, root.size());
	if (dirPath == "/")
		return (HttpStatus::FORBIDDEN);
	dirPath = root + dirPath;
	if (std::remove(dirPath.c_str())) {
		perror("std::remove");
		status = (errno == EACCES ? HttpStatus::FORBIDDEN : HttpStatus::SERVER_ERR);
	}
	return (status);
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

std::string getHeader(std::string request, std::string header) {
	std::string line;
	std::string value;
	size_t		headerLen = header.size();
	std::stringstream sstream(request);

	while (getline(sstream, line)) {
		if (line.substr(0, headerLen) == header) {
			value = line.substr(headerLen);
			trim(value, "\r");
			return value;
		}
	}
	return "";
}

ServerConfig getServer(std::vector<ServerConfig> serverConfigs, std::string host) {
	unsigned int	port = getRequestPort(host);
	std::vector<std::string>::iterator namesIt;
	std::vector<ServerConfig>::iterator it;

	host = host.substr(0, host.find(':'));
	if (host == "localhost" || host == "0.0.0.0")
		host = DEFAULT_HOST;
	for (it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
		std::vector<std::string> names = it->serverNames;
		for (namesIt = names.begin(); namesIt != names.end(); ++namesIt) {
			if (host == *namesIt && it->port == port)
				return *it;
		}
	}
	for (it = serverConfigs.begin(); it != serverConfigs.end(); it++)
		if (it->port == port)
			return *it;
	return serverConfigs.front();
}

static int hexStrToInt(std::string hexStr) {
	return (std::strtol(hexStr.c_str(), NULL, 16));
}

void Request::initRequest(std::string &request) {
	this->_fullRequest = request;
	this->_host = "";
	this->_contentType = "";
	this->_contentLength = 0;
	this->_body = "";
	bool transferEncodingChunked = false;
	std::vector<std::string>	requestLineParams = split(request, "\r\n");
	std::vector<std::string>::iterator	it = requestLineParams.begin();

	std::vector<std::string>	firstLine = split(*it, ' ');
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
			this->_contentLength += hexStrToInt(*it);
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

Request::Request(std::string request, std::vector<ServerConfig> serverConfigs, int connectionFd) : _connectionFd(connectionFd), _shouldRedirect(false), execCgi(false) {
	initRequest(request);
	_server = getServer(serverConfigs, this->_host);
	route = _server.getRouteByPath(this->_reqUri);
	_dirListEnabled = false;
	_shouldRedirect = false;

	if (route && route->path.size() <= this->_reqUri.size() ) { // localhost:8080/webserv would break with path /webserv/ because of substr below, figure out how to solve.
		_dirListEnabled = route->dirList;
		if (!route->redirect.first.empty()) {
			_shouldRedirect = this->_reqUri.substr(route->path.size()) == route->redirect.first;
			_locationHeader = "http://localhost:" + toString(_server.port) + route->path + route->redirect.second;
		}
	}
	if (route)
		_dirListEnabled = route->dirList;
	route ? filePath = getFilePath(route, this->_reqUri) : filePath = "";
	execCgi = shouldRunCgi(this->filePath, this->route->cgi);
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

HttpStatus::Code Request::runPost() {
	if (this->_contentType == "text/plain") {
		switch (checkPath(this->filePath)) {
			case ENOENT:
				break ;
			case EACCES:
				return (HttpStatus::FORBIDDEN);
			case ENOTDIR:
				return (HttpStatus::OK);
			default:
				return (HttpStatus::CONFLICT);
		}
		std::string prevPath = getPrevPath(filePath);
		switch (checkPath(prevPath)) {
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
		this->cgiOutput = getCgiOutput(this->filePath, this->_connectionFd, this->_body);
		this->resContentType = "text/html";

		return HttpStatus::OK;
	}
	return (HttpStatus::NOT_IMPLEMENTED);
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
	switch (fileGood(this->filePath.c_str())) {
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
	if (strEndsWith(_reqUri, '/')) { // example: /webserv/assets/style.css/  it is not a dir, so it wont trigger the condition above.
		if (!_dirListEnabled || access(filePath.c_str(), R_OK) == -1)
			status = HttpStatus::FORBIDDEN;
		else
			status = HttpStatus::NOT_FOUND;
		return (status);
	}
	if (this->execCgi) {
		this->cgiOutput = getCgiOutput(this->filePath, this->_connectionFd, "");
		this->resContentType = "text/plain";
	}
	return (HttpStatus::OK);
}

HttpStatus::Code	Request::runDelete() {
	struct stat	statbuf;

	if (checkParentFolderPermission(filePath, route->root))
		return (HttpStatus::FORBIDDEN);
	if (access(filePath.c_str(), F_OK))
		return (HttpStatus::NOT_FOUND);
	if (stat(filePath.c_str(), &statbuf)) {
		perror("stat");
		return (HttpStatus::SERVER_ERR);
	}
	if (S_ISDIR(statbuf.st_mode)) {
		if (strEndsWith(_reqUri, '/'))
			return deleteEverythingInsideDir(filePath, this->route->root);
		return (HttpStatus::CONFLICT);
	}
	return tryToDelete(filePath);
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
		default :
			return Response(runDelete(), *this);
	}
}
