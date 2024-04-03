#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>

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
			break;

		case UNKNOWNMETHOD:
			requestClass.setStatusCode(HttpStatus::NOTALLOWED);
			std::cout << "Couldn't recognize the method\n";
			break;
	}
}

Request::Request(std::string request, std::list<ServerConfig> serverConfigs) : _httpStatusCode(HttpStatus::OK), _serverConfigs(serverConfigs) {
	_method = ::getMethod(request);
	runMethods(_method, request, *this);
}

void Request::setStatusCode(HttpStatus::Code httpStatusCode) {
	_httpStatusCode = httpStatusCode;
}

HttpStatus::Code Request::getStatusCode() {
	return _httpStatusCode;
}

Methods Request::getMethod() {
	return _method;
}

void	Request::setFileContent(std::string fileContent) {
	_fileContent = fileContent;
}

std::string Request::getFileContent() {
	return _fileContent;
}

std::list<ServerConfig> Request::getServerConfigs() {
	return _serverConfigs;
}
