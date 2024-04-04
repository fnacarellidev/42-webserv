#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

std::vector<std::string> getRequestLineParams(std::string request) {
	std::string firstLine;
	std::stringstream strStream(request);

	getline(strStream, firstLine);

	return split(firstLine, ' ');
}

Methods getMethod(std::string method) {
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

void runGetMethod(std::string filePath, unsigned short allowedMethodsBitmask) {
	std::string fileContent;

	if (!(GET_OK & allowedMethodsBitmask)) {
		std::cout << "Not allowed to execute GET\n";
		/* requestClass.setStatusCode(HttpStatus::NOTALLOWED); */
		return ;
	}
	switch (fileGood(filePath.c_str())) {
		case ENOENT:
			/* requestClass.setStatusCode(HttpStatus::NOTFOUND); */
			break;

		case EACCES:
			/* requestClass.setStatusCode(HttpStatus::FORBIDDEN); */
			break;

		default:
			std::ifstream file;
			std::stringstream ss;

			file.open(filePath.c_str());
			ss << file.rdbuf();
			fileContent = ss.str();
			/* requestClass.setFileContent(fileContent); */
			break;
	}
}

void runDeleteMethod(std::string rootDir, std::string file, unsigned short allowedMethodsBitmask) {
	bool fileExists = false;
	DIR *d = opendir(rootDir.c_str());
	const std::string filePath = rootDir + file;
	const std::string noSlashFile = file.erase(0, 1);

	if (!(DELETE_OK & allowedMethodsBitmask)) {
		/* requestClass.setStatusCode(HttpStatus::NOTALLOWED); */
		return ;
	}
	else if (!d) {
		switch (errno) {
			case EACCES:
				/* requestClass.setStatusCode(HttpStatus::FORBIDDEN); */
				break;

			// ENOENT || ENOTDIR
			case ENOENT:
			case ENOTDIR:
				/* requestClass.setStatusCode(HttpStatus::NOTFOUND); */
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
		/* requestClass.setStatusCode(HttpStatus::NOCONTENT); */
	}
	/* else */
		/* requestClass.setStatusCode(HttpStatus::NOTFOUND); */
}

static bool pathIsDir(const char *filePath) {
	struct stat statbuf;

	if (stat(filePath, &statbuf) == -1) {
		perror("stat");
	}

	return S_ISDIR(statbuf.st_mode);
}

static std::string getFilePath(std::list<ServerConfig> serverConfigs, std::string requestUri) {
	std::list<RouteConfig> routeConfigs = serverConfigs.front().getRoutesConfigs();
	std::string root = routeConfigs.front().getRoot();

	return root + requestUri;
}

Request::Request(std::string request, std::list<ServerConfig> serverConfigs) : _serverConfigs(serverConfigs) {
	std::vector<std::string> requestLineParams = getRequestLineParams(request);

	_method = getMethod(requestLineParams[METHOD]);
	_filePath = getFilePath(_serverConfigs, requestLineParams[REQUESTURI]);
	_isDir = pathIsDir(requestLineParams[REQUESTURI].c_str());
}
