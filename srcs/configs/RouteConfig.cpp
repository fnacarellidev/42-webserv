#include "../../includes/RouteConfig.hpp"

static std::string	bitmaskToStr(unsigned short bitmask) {
	switch (bitmask) {
		case (GET_OK | POST_OK | DELETE_OK):
			return ("GET, POST, DELETE");
		case (GET_OK | POST_OK):
			return ("GET, POST");
		case (GET_OK | DELETE_OK):
			return ("GET, DELETE");
		case (POST_OK | DELETE_OK):
			return ("POST, DELETE");
		case POST_OK:
			return ("POST");
		case DELETE_OK:
			return ("DELETE");
		default:
			return ("GET");
	}
}

RouteConfig::RouteConfig():
path(DEFAULT_PATH), root(DEFAULT_ROOT), acceptMethodsBitmask(DEFAULT_ACCEPT_METHODS), dirList(DEFAULT_DIRLIST) {
	index.push_back(DEFAULT_INDEX);
}

std::ostream&	operator<<(std::ostream& os, RouteConfig& routeConfig) {
	os << "\t\tpath: " << routeConfig.path << std::endl;
	os << "\t\tredirect: " << routeConfig.redirect.first << " -> " << routeConfig.redirect.second << std::endl;
	os << "\t\tindex: ";
	for (std::vector<std::string>::iterator it = routeConfig.index.begin(); it != routeConfig.index.end(); ++it)
		os << *it << (it + 1 != routeConfig.index.end() ? ", " : "");
	os << std::endl;
	os << "\t\troot: " << routeConfig.root << std::endl;
	os << "\t\tacceptMethodsBitmask: " << bitmaskToStr(routeConfig.acceptMethodsBitmask) << std::endl;
	os << "\t\tdirList: " << routeConfig.dirList << std::endl;
	return os;
}
