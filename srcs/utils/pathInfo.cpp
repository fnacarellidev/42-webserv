#include "../../includes/utils.hpp"

struct stat	utils::pathInfo(const std::string &path) {
	struct stat info;
	stat(path.c_str(), &info);
	return (info);
}

bool utils::pathExists(const std::string &dir) {
	struct stat statbuff;
	return (stat(dir.c_str(), &statbuff) == 0);
}

std::string	utils::getPrevPath(const std::string &fullPath) {
	if (fullPath[fullPath.size() - 1] == '/' || fullPath.rfind("/") == std::string::npos)
		return (fullPath);
	return (fullPath.substr(0, fullPath.rfind("/")));
}

int	utils::checkPath(std::string &path) {
	if (!utils::pathExists(path))
		return (ENOENT);
	if (access(path.c_str(), R_OK) != 0)
		return (EACCES);
	if (!S_ISDIR(utils::pathInfo(path.c_str()).st_mode))
		return (ENOTDIR);
	return (0);
}
