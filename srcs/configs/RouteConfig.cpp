#include "../../includes/RouteConfig.hpp"

RouteConfig::RouteConfig() {
// redirect n tem default
	_root = DEFAULT_ROOT;
	_index.push_back(DEFAULT_INDEX);
	_dirList = DEFAULT_DIRLIST;
	_acceptMethodsBitmask = DEFAULT_ACCEPT_METHODS;
}

void RouteConfig::setRoot(std::string root) {
	_root = root;
}

void RouteConfig::setIndex(std::vector<std::string> index) {
	_index = index;
}

void RouteConfig::setDirList(bool dirList) {
	_dirList = dirList;
}

void RouteConfig::setRedirect(std::pair<std::string, std::string> redirect) {
	_redirect = redirect;
}

void RouteConfig::setAcceptMethodsBitmask(unsigned short acceptMethodsBitmask) {
	_acceptMethodsBitmask = acceptMethodsBitmask;
}

bool RouteConfig::getDirList() {
	return _dirList;
}

std::string RouteConfig::getRoot() {
	return _root;
}

std::vector<std::string> RouteConfig::getIndex() {
	return _index;
}

std::pair<std::string, std::string> RouteConfig::getRedirect() {
	return _redirect;
}

unsigned short RouteConfig::getAcceptMethodsBitmask(){
	return _acceptMethodsBitmask;
}