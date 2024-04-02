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

Methods getMethod(std::string request) {
	Methods method = INVALMETHOD;
	std::vector<std::string> requestLineParams = getRequestLineParams(request);
	std::map<std::string, Methods> methodsMap = getMethodsMap();
	std::string methodKey = requestLineParams[METHOD];

	if (methodsMap.find(methodKey) != methodsMap.end())
		method = methodsMap[methodKey];

	return method;
}

Request::Request(std::string request) {
	std::cout << getMethod(request) << std::endl;
}
