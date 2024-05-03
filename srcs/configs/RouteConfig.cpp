#include "../../includes/RouteConfig.hpp"

RouteConfig::RouteConfig():
path(DEFAULT_PATH), root(DEFAULT_ROOT), acceptMethodsBitmask(DEFAULT_ACCEPT_METHODS), dirList(DEFAULT_DIRLIST) {
	index.push_back(DEFAULT_INDEX);
}
