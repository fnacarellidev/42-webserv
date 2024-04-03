#include "../includes/Response.hpp"
#include <ctime>
#include <sys/stat.h>
#include <map>
#include <ctime>

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

static time_t	convertTimeToGMT(time_t t) {
	struct tm	*gmtTime = gmtime(&t);
	return (mktime(gmtTime));
}

static std::string	formatTimeString(time_t	time) {
	char	buffer[80];
	std::strftime(buffer, sizeof(buffer), "%c", localtime(&time));
	std::string strTime(buffer);
	strTime += " GMT";
	return (strTime);
}

static std::string	getCurrentTimeInGMT() {
	time_t	now = convertTimeToGMT(time(0));
	return (formatTimeString(now));
}

static std::string	getLastModifiedOfFile(const std::string &filename) {
	struct stat stat_buff;
	stat(filename.c_str(), &stat_buff);
	time_t	gmtTime = convertTimeToGMT(stat_buff.st_mtime);
	return (formatTimeString(gmtTime));
}

static std::string	getFileSize(std::string filename) {
	struct stat	stat_buff;
	stat(filename.c_str(), &stat_buff);
	return (toString(stat_buff.st_size));
}

static std::string	getContentType(std::string filename) {
	std::map<std::string, std::string>	mimeTypes;

	mimeTypes[".html"] = "text/html";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".css"] = "text/css";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".gif"] = "image/gif";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".php"] = "application/x-httpd-php";
	std::string	extension = filename.substr(filename.find_last_of("."));
	std::map<std::string, std::string>::iterator	it = mimeTypes.find(extension);
	if (it != mimeTypes.end())
		return (it->second);
	else
		return ("Content Type Unknown");
}

void	Response::_success() {
	this->_header.push_back(HTTP_VERSION + (" " + toString(this->_status) + " OK"));
	this->_header.push_back("Date: " + getCurrentTimeInGMT());
	this->_header.push_back("Server: Webserv/1.0");
	this->_header.push_back("ETag: ");
	if (!this->_bodyFile.empty()) {
		this->_header.push_back("Last-Modified: " + getLastModifiedOfFile(this->_bodyFile));
		this->_header.push_back("Content-Lenght: " + getFileSize(this->_bodyFile));
		this->_header.push_back("Content-Type: " + getContentType(this->_bodyFile));
	}
	this->_header.push_back("Connection: keep-alive");
}
