#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <sstream>
#include <string>
#include <map>

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

Request::Request(std::string request) : _httpStatusCode(HttpStatus::OK) {
	_method = ::getMethod(request, *this);
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
