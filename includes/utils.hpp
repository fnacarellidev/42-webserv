#pragma once

#include "includeAll.hpp"
#include "HttpStatus.hpp"
#include "WebServer.hpp"

class RouteConfig;
class ServerConfig;

namespace utils {
	std::string	getFileContent(const std::string &filename);
	std::string	getLastModifiedOfFile(const std::string &filename);
	std::string	getFileSize(const std::string &filename);
	struct stat	pathInfo(const std::string &path);
	bool	pathExists(const std::string &dir);
	std::string	getPrevPath(const std::string &fullPath);
	int	checkPath(std::string &path);
	std::vector<std::string>	split(std::string str, char c);
	std::vector<std::string>	split(std::string str, std::string c);
	char*	strdup(std::string str);
	bool	strEndsWith(std::string str, char c);
	time_t	convertTimeToGMT(time_t t);
	std::string	formatTimeString(time_t	time);
	template<typename T> std::string	toString(const T& value);
	std::string	getCurrentTimeInGMT();
	void	trim(std::string &str, const char* toFind);
	void	sleep(int seconds);
	int		fileGood(const char *filePath);
	bool	checkParentFolderPermission(std::string filePath, const std::string& root);
	std::string getFilePath(RouteConfig *route, std::string requestUri);
	std::string getHeader(std::string request, std::string header);
	ServerConfig getServer(std::vector<ServerConfig> serverConfigs, std::string host);
	HttpStatus::Code	deleteEverythingInsideDir(std::string dirPath, std::string& root);
	HttpStatus::Code	tryToDelete(const std::string& filePath);
	void				getDateAndBytes(const std::string &path, std::string &modTime, std::string &bytesSize);
	void				setupPolls(std::vector<int>& serverFds, std::vector<struct pollfd>& pollFds);
	void				setupSockets(WebServer& config, std::vector<int>& serverFds) throw(std::runtime_error);
}

#include "../srcs/utils/toString.tpp"

