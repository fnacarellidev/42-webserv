#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

std::vector<std::string> getRequestLineParams(std::string request) {
	std::string firstLine;
	std::stringstream strStream(request);

	getline(strStream, firstLine);

	return split(firstLine, ' ');
}

Methods getMethod(std::string request) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);
	std::string method = requestLineParams[METHOD];

	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	return UNKNOWNMETHOD;
}

int	fileGood(const char *filePath) {
	bool fileExists = !access(filePath, F_OK);
	bool canRead = !access(filePath, R_OK);

	if (!fileExists)
		return ENOENT;
	else if (!canRead)
		return EACCES;
	return 0;
}

void runGetMethod(std::string filePath, unsigned short allowedMethodsBitmask, Request &requestClass) {
	std::string fileContent;

	if (!(GET_OK & allowedMethodsBitmask)) {
		std::cout << "Not allowed to execute GET\n";
		requestClass.setStatusCode(HttpStatus::NOTALLOWED);
		return ;
	}
	switch (fileGood(filePath.c_str())) {
		case ENOENT:
			requestClass.setStatusCode(HttpStatus::NOTFOUND);
			break;

		case EACCES:
			requestClass.setStatusCode(HttpStatus::FORBIDDEN);
			break;

		default:
			std::ifstream file;
			std::stringstream ss;

			file.open(filePath.c_str());
			ss << file.rdbuf();
			fileContent = ss.str();
			requestClass.setFileContent(fileContent);
			break;
	}
}

void runDeleteMethod(std::string rootDir, std::string file, unsigned short allowedMethodsBitmask, Request &requestClass) {
	bool fileExists = false;
	DIR *d = opendir(rootDir.c_str());
	const std::string filePath = rootDir + file;
	const std::string noSlashFile = file.erase(0, 1);

	if (!(DELETE_OK & allowedMethodsBitmask)) {
		requestClass.setStatusCode(HttpStatus::NOTALLOWED);
		return ;
	}
	else if (!d) {
		switch (errno) {
			case EACCES:
				requestClass.setStatusCode(HttpStatus::FORBIDDEN);
				break;

			// ENOENT || ENOTDIR
			case ENOENT:
			case ENOTDIR:
				requestClass.setStatusCode(HttpStatus::NOTFOUND);
				break ;
		};
		return ;
	}
	for (struct dirent *dir = readdir(d); dir != NULL; dir = readdir(d)) {
		if (dir->d_name == noSlashFile) {
			fileExists = true;
			break ;
		}
	}
	closedir(d);
	if (fileExists) {
		remove(filePath.c_str());
		requestClass.setStatusCode(HttpStatus::NOCONTENT);
	}
	else
		requestClass.setStatusCode(HttpStatus::NOTFOUND);
}

void	runMethods(Methods method, std::string request, Request &requestClass) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);
	std::string requestUri = requestLineParams[REQUESTURI];
	std::list<ServerConfig> serverConfigs = requestClass.getServerConfigs();
	std::list<RouteConfig> routeConfigs = serverConfigs.front().getRoutesConfigs();
	unsigned short allowedMethodsBitmask = routeConfigs.front().getAllowedMethodsBitmask();
	std::string filePath = routeConfigs.front().getRoot() + requestUri;

	switch (method) {
		case GET:
			runGetMethod(filePath, allowedMethodsBitmask, requestClass);
			break;

		case POST:
			break;

		case DELETE:
			runDeleteMethod(routeConfigs.front().getRoot(), requestUri, allowedMethodsBitmask, requestClass);
			break;

		case UNKNOWNMETHOD:
			requestClass.setStatusCode(HttpStatus::NOTALLOWED);
			break;
	}
	if (method != UNKNOWNMETHOD)
		std::cout << requestLineParams[METHOD] << " exited with status code " << requestClass.getStatusCode() << std::endl;
	else
		std::cout << requestLineParams[METHOD] << " is unknown and exited with status code " << requestClass.getStatusCode() << std::endl;
}

Request::Request(std::string request, std::list<ServerConfig> serverConfigs) : _httpStatusCode(HttpStatus::OK), _serverConfigs(serverConfigs) {
	_method = ::getMethod(request);
	runMethods(_method, request, *this);
}

void Request::setStatusCode(HttpStatus::Code httpStatusCode) {
	_httpStatusCode = httpStatusCode;
}

HttpStatus::Code Request::getStatusCode() const {
	return _httpStatusCode;
}

Methods Request::getMethod() const {
	return _method;
}

void	Request::setFileContent(std::string fileContent) {
	_fileContent = fileContent;
}

std::string Request::getFileContent() const {
	return _fileContent;
}

std::list<ServerConfig> Request::getServerConfigs() const {
	return _serverConfigs;
}
