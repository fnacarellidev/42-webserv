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

int	utils::fileGood(const char *filePath) {
	bool fileExists = !access(filePath, F_OK);
	bool canRead = !access(filePath, R_OK);

	if (!fileExists)
		return ENOENT;
	else if (!canRead)
		return EACCES;
	return 0;
}

bool utils::checkParentFolderPermission(std::string filePath, const std::string& root) {
	size_t fileSlashes = std::count(filePath.begin(), filePath.end(), '/');
	size_t rootSlashes = std::count(root.begin(), root.end(), '/');

	while (fileSlashes-- > rootSlashes) {
		filePath = utils::getPrevPath(filePath);
		if (access(filePath.c_str(), R_OK | W_OK | X_OK) == -1) {
			perror("access");
			return true;
		}
	}
	return false;
}

