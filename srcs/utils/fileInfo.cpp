#include "../../includes/utils.hpp"
#include "../../includes/Request.hpp"

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

HttpStatus::Code	utils::tryToDelete(const std::string& filePath) {
	if (std::remove(filePath.c_str())) {
		perror("std::remove");
		return (HttpStatus::SERVER_ERR);
	}
	return (HttpStatus::NO_CONTENT);
}

HttpStatus::Code	utils::deleteEverythingInsideDir(std::string dirPath, std::string& root) {
	HttpStatus::Code	status = HttpStatus::NO_CONTENT;
	DIR	*dir = opendir(dirPath.c_str());

	if (dir == NULL) {
		perror("opendir");
		return (HttpStatus::SERVER_ERR);
	}
	dirPath += '/';
	for (struct dirent *item = readdir(dir); item != NULL; item = readdir(dir)) {
		std::string filePath;

		if (std::strcmp(item->d_name, ".") == 0 || std::strcmp(item->d_name, "..") == 0)
			continue;
		filePath = dirPath + item->d_name;
		if (std::remove(filePath.c_str())) {
			perror("std::remove");
			status = (errno == EACCES ? HttpStatus::FORBIDDEN : HttpStatus::SERVER_ERR);
		}
	}
	closedir(dir);
	dirPath.erase(0, root.size());
	if (dirPath == "/")
		return (HttpStatus::FORBIDDEN);
	dirPath = root + dirPath;
	if (std::remove(dirPath.c_str())) {
		perror("std::remove");
		status = (errno == EACCES ? HttpStatus::FORBIDDEN : HttpStatus::SERVER_ERR);
	}
	return (status);
}


void	utils::getDateAndBytes(const std::string &path, std::string &modTime, std::string &bytesSize) {
	struct stat	fileStat;
	struct tm	*timeInfo;
	char	buffer[20];

	stat(path.c_str(), &fileStat);
	timeInfo = gmtime(&fileStat.st_mtime);
	strftime(buffer, 20, "%d-%b-%Y %H:%M", timeInfo);
	modTime = buffer;
	bytesSize = utils::toString(fileStat.st_size);
}
