#pragma once

#include "includeAll.hpp"

std::string	getFileContent(const std::string &filename);
std::string	getLastModifiedOfFile(const std::string &filename);
std::string	getFileSize(const std::string &filename);
time_t		convertTimeToGMT(time_t t);
std::string	formatTimeString(time_t	time);
std::string	getCurrentTimeInGMT();
std::vector<std::string> split(std::string str, char c);
std::vector<std::string> split(std::string str, std::string c);
void	trim(std::string &str, const char* toFind);
struct stat	pathInfo(const std::string &path);
bool pathExists(const std::string &dir);
std::string	getPrevPath(const std::string &fullPath);
int	checkPath(std::string &path);
bool strEndsWith(std::string str, char c);

template<typename T>
std::string	toString(const T& value) {
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}
