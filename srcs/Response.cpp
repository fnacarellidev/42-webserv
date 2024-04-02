#include "../includes/Response.hpp"
#include <ctime>
#include <sys/stat.h>
#include <map>

Response::Response() {
	this->_status = 0;
	this->_body = "";
}

Response::Response(short int status, std::string bodyFile) {
	this->_status = status;
	this->_bodyFile = bodyFile;
	switch (status / 100) { 
		case 1: // 1xx
			break ;
		case 2: // 2xx
			this->_success();
			break ;
		case 3: // 3xx
			break ;
		case 4: // 4xx
			break ;
		case 5: // 5xx
			break ;
	}
}

Response::Response(short int status) {
	this->_status = status;
}

Response::~Response() {}

static std::string	getFileContent(std::string filename) {
	std::ifstream	file;
	std::string		content;

	file.open(filename.c_str());
	std::getline(file, content, '\0');
	file.close();
	return(content);
}

std::string	Response::getFullReponse() {
	std::string	fullReponse = "";
	std::vector<std::string>::iterator	it;

	for (it = this->_header.begin(); it != this->_header.end(); it++)
		fullReponse += *it + '\n';
	fullReponse += '\n' + getFileContent(this->_bodyFile);
	return (fullReponse);
}

static std::string	getCurrentTime() {
	time_t	now = time(0);
	std::string	currentTime = ctime(&now);
	return (currentTime.substr(0, currentTime.length() - 1));
}

static std::string	getLastModifiedOfFile(const std::string &filename) {
	struct stat stat_buff;
	stat(filename.c_str(), &stat_buff);
	return (toString(stat_buff.st_mtime));
}

static std::string	getFileSize(std::string filename) {
	struct stat	stat_buff;
	stat(filename.c_str(), &stat_buff);
	return (toString(stat_buff.st_size));
}

static std::string	getContentType(std::string filename)
{
	std::map<std::string, std::string>	mimeTypes;

	mimeTypes["html"] = "text/html";
	std::string	extension = filename.substr(filename.find_last_of(".") + 1);
	std::map<std::string, std::string>::iterator	it = mimeTypes.find(extension);
	if (it != mimeTypes.end())
		return (it->second);
	else
		return ("Content Type Unknown");
}

void	Response::_success() {
	this->_header.push_back(HTTP_VERSION + (" " + toString(this->_status) + " OK"));
	this->_header.push_back("Date: " + getCurrentTime());
	this->_header.push_back("Server: Webserv/1.0");
	this->_header.push_back("ETag: ");
	if (!this->_bodyFile.empty()) {
		this->_header.push_back("Last-Modified: " + getLastModifiedOfFile(this->_bodyFile));
		this->_header.push_back("Content-Lenght: " + getFileSize(this->_bodyFile));
		this->_header.push_back("Content-Type: " + getContentType(this->_bodyFile));
	}
	this->_header.push_back("Connection: keep-alive");
}
