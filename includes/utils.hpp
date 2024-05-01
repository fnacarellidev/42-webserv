#pragma once

#include "includeAll.hpp"

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
}

#include "../srcs/utils/toString.tpp"

