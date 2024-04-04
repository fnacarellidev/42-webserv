#include "../includes/Response.hpp"

Response::Response() {
	this->_status = 0;
	this->_body = "";
}

Response::Response(short int status, std::string bodyFile) {
	this->_status = status;
	this->_bodyFile = bodyFile;
	switch (status / 100) { 
		// case 1: // 1xx
		//	this->_informatiol
		// 	break ;
		case 2: // 2xx
			this->_success();
			break ;
		case 3: // 3xx
			this->_redirection();
			break ;
		case 4: // 4xx
			this->_error();
			break ;
		case 5: // 5xx
			this->_serverError();
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
	std::vector<t_fields>::iterator	it;

	fullReponse += this->_statusLine + "\n";
	for (it = this->_header.begin(); it != this->_header.end(); it++)
		fullReponse += it->first + ": " + it->second + "\n";
	fullReponse += '\n' + this->_body;
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

static std::string	getStatusMessage(int status) {
	std::map<int, std::string>	statusMessages;

	statusMessages[200] = "OK";
	statusMessages[201] = "Created";
	statusMessages[204] = "No Content";
	statusMessages[301] = "Moved Permanently";
	statusMessages[302] = "Found";
	statusMessages[304] = "Not Modified";
	statusMessages[400] = "Bad Resquest";
	statusMessages[401] = "Unauthorized";
	statusMessages[403] = "Forbidden";
	statusMessages[404] = "Not Found";
	statusMessages[405] = "Method Not Allowed";
	statusMessages[408] = "Request Timeout";
	statusMessages[500] = "Internal Server Error";
	statusMessages[502] = "Bad Gateway";
	statusMessages[503] = "Service Unavailable";
	std::map<int, std::string>::iterator	it = statusMessages.find(status);
	std::string	statusMessage;
	if (it != statusMessages.end())
		statusMessage = toString(status) + " " + it->second;
	else
		statusMessage = toString(status) + " Unknown Status";
	return (statusMessage);
}

static std::string	defineStatusLine(int status) {
	std::string	statusLine;
	statusLine = HTTP_VERSION + (" " + toString(status) + " " + getStatusMessage(status));
	return (statusLine);
}

void	Response::_success() {
	this->_statusLine = defineStatusLine(this->_status);
	this->_header.push_back(std::make_pair("Date", getCurrentTimeInGMT()));
	this->_header.push_back(std::make_pair("Server", SERVER_NAME));
	this->_header.push_back(std::make_pair("Etag", "* hash function *"));
	switch (this->_status) {
		case 200:
			this->_header.push_back(std::make_pair("Last-Modified", getLastModifiedOfFile(this->_bodyFile)));
			this->_header.push_back(std::make_pair("Content-Lenght", getFileSize(this->_bodyFile)));
			this->_header.push_back(std::make_pair("Content-Type", getContentType(this->_bodyFile)));
			this->_body = getFileContent(this->_bodyFile);
			break ;
		case 201:
			this->_header.push_back(std::make_pair("Location:", "/path/to/some?"));
			this->_body = getFileContent(this->_bodyFile);
			break ;
		case 204:
			break ;
	}
	// this->_header.push_back(std::make_pair("Connection", "keep-alive"));
}

void	Response::_redirection() {
	this->_statusLine = defineStatusLine(this->_status);
	this->_header.push_back(std::make_pair("Date", getCurrentTimeInGMT()));
	this->_header.push_back(std::make_pair("Server", SERVER_NAME));
	this->_header.push_back(std::make_pair("Etag", "* hash function *"));
	switch (this->_status) {
		case 301:
			this->_header.push_back(std::make_pair("Location:", "/path/to/some?"));
			break ;
		case 302:
			this->_header.push_back(std::make_pair("Location:", "/path/to/some?"));
			break ;
		case 304:
			break ;
	}
}

void	Response::_error(){
	this->_statusLine = defineStatusLine(this->_status);
	this->_header.push_back(std::make_pair("Date", getCurrentTimeInGMT()));
	this->_header.push_back(std::make_pair("Server", SERVER_NAME));
	this->_header.push_back(std::make_pair("Etag", "* hash function *"));
	this->_header.push_back(std::make_pair("Content-Lenght", getFileSize(this->_bodyFile)));
	this->_header.push_back(std::make_pair("Content-Type", getContentType(this->_bodyFile)));
	this->_body = getFileContent(this->_bodyFile);
	// switch (this->_status) {
	// 	case 400:
	// 		break ;
	// 	case 401:
	// 		break ;
	// 	case 403:
	// 		break ;
	// 	case 404:
	// 		break ;
	// 	case 405:
	// 		break ;
	// 	case 408:
	// 		break ;
	// }
}

void	Response::_serverError() {
	return ;
}
