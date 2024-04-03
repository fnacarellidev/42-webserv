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

std::map<std::string, Methods> getMethodsMap() {
	std::map<std::string, Methods> methodsMap;

	methodsMap["GET"] = GET;
	methodsMap["POST"] = POST;
	methodsMap["DELETE"] = DELETE;

	return methodsMap;
}

Methods getMethod(std::string request, Request &requestClass) {
	Methods method = INVALMETHOD;
	std::vector<std::string> requestLineParams = getRequestLineParams(request);
	std::map<std::string, Methods> methodsMap = getMethodsMap();
	std::string methodKey = requestLineParams[METHOD];

	if (methodsMap.find(methodKey) == methodsMap.end())
		requestClass.setStatusCode(HttpStatus::NOTALLOWED);
	method = methodsMap[methodKey];

	return method;
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

void runGetMethod(std::string request, Request &requestClass) {
	std::string fileContent;
	std::vector<std::string> requestLineParams = getRequestLineParams(request);
	std::string filePath = requestClass.getRootPath() + requestLineParams[REQUESTURI];

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

Request::Request(std::string request, std::string rootPath) : _httpStatusCode(HttpStatus::OK), _rootPath(rootPath) {
	_method = ::getMethod(request, *this);
	switch (_method) {
		case GET:
			runGetMethod(request, *this);
			break;

		case POST:
			break;

		case DELETE:
			break;

		case INVALMETHOD:
			break;
	}
	std::cout << "STATUS CODE: " << _httpStatusCode << std::endl;
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

std::string Request::getRootPath() {
	return _rootPath;
}

void	Request::setFileContent(std::string fileContent) {
	_fileContent = fileContent;
}

std::string Request::getFileContent() {
	return _fileContent;
}
