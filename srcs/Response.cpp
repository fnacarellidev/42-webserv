#include "../includes/Response.hpp"

static void	getDateAndBytes(const std::string &path, std::string &modTime, std::string &bytesSize) {
	struct stat	fileStat;
	struct tm	*timeInfo;
	char	buffer[20];

	stat(path.c_str(), &fileStat);
	timeInfo = gmtime(&fileStat.st_mtime);
	strftime(buffer, 20, "%d-%b-%Y %H:%M", timeInfo);
	modTime = buffer;
	bytesSize = toString(fileStat.st_size);
}

static std::string	generateDirectoryListing(const std::string &path) {
	std::string	dirListing;
	DIR	*dir = opendir(path.c_str());

	dirListing += "<html><head><title>Directory listing for " + path + "</title></head><body>";
	dirListing += "<h1>Directory listing for " + path + "</h1>";
	dirListing += "<hr><pre>";
	dirListing += "<a href=\"..\">../</a>\n";
	for (struct dirent *item = readdir(dir); item != NULL; item = readdir(dir)) {
		if (std::strcmp(item->d_name, ".") == 0 || std::strcmp(item->d_name, "..") == 0)
			continue ;

		std::string modTime, bytesSize, file = item->d_name;

		getDateAndBytes(path + item->d_name, modTime, bytesSize);
		dirListing += "<a href=\"" + file + "\">" + file;
		if (item->d_type == DT_DIR) {
			dirListing += "/";
			dirListing += "</a>";
			dirListing.append(255 - file.size() - 1, ' ');
			dirListing += modTime;
			dirListing.append(19, ' ');
			dirListing += "-\n";
		} else {
			dirListing += "</a>";
			dirListing.append(255 - file.size(), ' ');
			dirListing += modTime;
			dirListing.append(20 - bytesSize.size(), ' ');
			dirListing += bytesSize + "\n";
		}
	}
	closedir(dir);
	dirListing += "</pre><hr></body></html>";
	return (dirListing);
}

static std::string	getErrInformation(int status)
{
	switch (status) {
		case 400:
			return (E400);
		case 401:
			return (E401);
		case 403:
			return (E403);
		case 404:
			return (E404);
		case 405:
			return (E405);
		case 409:
			return (E409);
		case 413:
			return (E413);
		case 500:
			return (E500);
		case 501:
			return (E501);
		case 502:
			return (E502);
		case 503:
			return (E503);
		default:
			return ("");
	}
}

static std::string	generateDefaultErrorPage(int status, const std::string &statusMsg) {
	std::string	errorPage = "";
	errorPage += "<html>\n";
	errorPage += "<head><title>" + statusMsg + "</title></head>\n";
	errorPage += "<body>\n";
	errorPage += "<h1>" + statusMsg + "</h1>\n";
	errorPage += "<p>" + getErrInformation(status) + "</p>\n";
	errorPage += "</body>\n";
	errorPage += "</html>\n";
	return (errorPage);
}

static std::map<std::string, std::string>	defaultMimeTypes() {
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
	return (mimeTypes);
}

static std::map<int, std::string>	defaultStatusMessages() {
	std::map<int, std::string>	statusMessages;
	statusMessages[200] = "OK";
	statusMessages[201] = "Created";
	statusMessages[204] = "No Content";
	statusMessages[301] = "Moved Permanently";
	statusMessages[302] = "Found";
	statusMessages[304] = "Not Modified";
	statusMessages[400] = "Bad Request";
	statusMessages[401] = "Unauthorized";
	statusMessages[403] = "Forbidden";
	statusMessages[404] = "Not Found";
	statusMessages[405] = "Method Not Allowed";
	statusMessages[408] = "Request Timeout";
	statusMessages[409] = "Conflict";
	statusMessages[413] = "Payload Too Large";
	statusMessages[500] = "Internal Server Error";
	statusMessages[501] = "Not Implemented";
	statusMessages[502] = "Bad Gateway";
	statusMessages[503] = "Service Unavailable";
	return (statusMessages);
}

Response::Response(int status) {
	this->_status = status;
	this->_bodyFile = "";
	this->_mimeTypes = defaultMimeTypes();
	this->_statusMessages = defaultStatusMessages();
	this->defineStatusLine(status);
	// Shouldn't ever fall on case 3 since it will never be called with a 300 status.
	switch (status / 100) {
		case 2:
			this->_success();
			break ;
		case 4:
			this->_error();
			break ;
		case 5:
			this->_serverError();
			break ;
	}
	this->generateFullResponse();
}

Response::Response(int status, std::string bodyFile) {
	this->_status = status;
	this->_bodyFile = (status == 301 ? bodyFile.append("/") : bodyFile);
	this->_mimeTypes = defaultMimeTypes();
	this->_statusMessages = defaultStatusMessages();
	this->defineStatusLine(status);
	// Shouldn't ever fall on case 3 since it will never be called with a 300 status.
	switch (status / 100) {
		case 2:
			this->_success();
			break ;
		case 4:
			this->_error();
			break ;
		case 5:
			this->_serverError();
			break ;
	}
	this->generateFullResponse();
}

Response::Response(int status, std::string bodyFile, std::string locationHeader) {
	this->_status = status;
	this->_bodyFile = (status == 301 ? bodyFile.append("/") : bodyFile);
	this->_mimeTypes = defaultMimeTypes();
	this->_statusMessages = defaultStatusMessages();
	this->defineStatusLine(status);
	this->_redirection(locationHeader);
	this->generateFullResponse();
}


Response	&Response::operator=(const Response &other) {
	if (this != &other) {
		this->_body = other._body;
		this->_status = other._status;
		this->_bodyFile = other._bodyFile;
		this->_mimeTypes = other._mimeTypes;
		this->_statusLine = other._statusLine;
		this->_headerFields = other._headerFields;
		this->_fullResponse = other._fullResponse;
		this->_statusMessages = other._statusMessages;
	}
	return (*this);
}

std::string	Response::getFullResponse() const {
	return (this->_fullResponse);
}

const char	*Response::response() const {
	return (this->_fullResponse.c_str());
}

size_t		Response::size() const {
	return (this->_fullResponse.length());
}

std::string	Response::getContentType(const std::string &filename) const {
	std::map<std::string, std::string>::const_iterator	it;
	std::string	extension;
	if (filename.empty())
		return ("text/plain");
	if (filename.find_last_of(".") == std::string::npos)
		return (*(filename.end() - 1) == '/' ? "text/html" : "text/plain");
	extension = filename.substr(filename.find_last_of("."));
	it = this->_mimeTypes.find(extension);
	if (it != this->_mimeTypes.end())
		return (it->second);
	else
		return ("Content Type Unknown");
}

std::string	Response::getStatusMessage(int status) const {
	std::map<int, std::string>::const_iterator	it;
	std::string	statusMessage;
	it = this->_statusMessages.find(status);
	if (it != this->_statusMessages.end())
		statusMessage = toString(status) + " " + it->second;
	else
		statusMessage = toString(status) + " Unknown Status";
	return (statusMessage);
}

void	Response::generateFullResponse() {
	std::vector<t_fields>::iterator	it;

	this->_fullResponse += this->_statusLine + "\n";
	for (it = this->_headerFields.begin(); it != this->_headerFields.end(); it++)
		this->_fullResponse += it->first + ": " + it->second + "\n";
	this->_fullResponse += '\n' + this->_body;
}

void	Response::defineStatusLine(int status) {
	this->_statusLine = HTTP_VERSION + (" " + getStatusMessage(status));
}

void	Response::addNewField(std::string key, std::string value) {
	this->_headerFields.push_back(std::make_pair(key, value));
}

void	Response::_success() {
	struct stat fileInfo;
	stat(this->_bodyFile.c_str(), &fileInfo);
	this->addNewField("Date", getCurrentTimeInGMT());
	this->addNewField("Server", SERVER_NAME);
	if (this->_bodyFile.empty())
		return ;
	switch (this->_status) {
		case 200:
			if (!S_ISDIR(fileInfo.st_mode)){
				this->addNewField("Last-Modified", getLastModifiedOfFile(this->_bodyFile));
				this->addNewField("Content-Length", getFileSize(this->_bodyFile));
				this->addNewField("Content-Type", getContentType(this->_bodyFile));
				this->_body = getFileContent(this->_bodyFile);
			} else {
				this->addNewField("Content-Type", "text/html");
				this->_body = generateDirectoryListing(this->_bodyFile);
			}
			break ;
		case 201:
			this->addNewField("Content-Type", getContentType(this->_bodyFile));
			this->_body = "File created";
			break ;
		case 204:
			break ;
	}
}

void	Response::_redirection(std::string locationHeader) {
	this->addNewField("Date", getCurrentTimeInGMT());
	this->addNewField("Server", SERVER_NAME);
	if (this->_bodyFile.empty())
		return ;
	addNewField("Location", locationHeader);
}

void	Response::_error() {
	this->addNewField("Date", getCurrentTimeInGMT());
	this->addNewField("Server", SERVER_NAME);
	if (!this->_bodyFile.empty()) {
		this->_body = getFileContent(this->_bodyFile);
		this->addNewField("Content-Length", getFileSize(this->_bodyFile));
		this->addNewField("Content-Type", getContentType(this->_bodyFile));
	} else {
		std::string statusMsg = getStatusMessage(this->_status);
		this->_body = generateDefaultErrorPage(this->_status, statusMsg);
		this->addNewField("Content-Length", toString(this->_body.size()));
		this->addNewField("Content-Type", "text/html");
	}
}

void	Response::_serverError() {
	this->addNewField("Date", getCurrentTimeInGMT());
	this->addNewField("Server", SERVER_NAME);
	if (!this->_bodyFile.empty()) {
		this->_body = getFileContent(this->_bodyFile);
		this->addNewField("Content-Length", getFileSize(this->_bodyFile));
		this->addNewField("Content-Type", getContentType(this->_bodyFile));
	} else {
		std::string statusMsg = getStatusMessage(this->_status);
		this->_body = generateDefaultErrorPage(this->_status, statusMsg);
		this->addNewField("Content-Length", toString(this->_body.size()));
		this->addNewField("Content-Type", "text/html");
	}
}
