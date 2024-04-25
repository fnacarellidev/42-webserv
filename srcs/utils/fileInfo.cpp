#include "../../includes/utils.hpp"

std::string	utils::getFileContent(const std::string &filename) {
	std::ifstream	file;
	std::string		content;

	file.open(filename.c_str());
	std::getline(file, content, '\0');
	file.close();
	return(content);
}

std::string	utils::getLastModifiedOfFile(const std::string &filename) {
	struct stat stat_buff;
	stat(filename.c_str(), &stat_buff);
	time_t	gmtTime = convertTimeToGMT(stat_buff.st_mtime);
	return (formatTimeString(gmtTime));
}

std::string	utils::getFileSize(const std::string &filename) {
	struct stat	stat_buff;
	stat(filename.c_str(), &stat_buff);
	return (toString(stat_buff.st_size));
}
