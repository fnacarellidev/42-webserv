#include "../includes/RouteConfig.hpp"

RouteConfig::RouteConfig() {
	_allowedMethodsBitmask = GET_OK | POST_OK | DELETE_OK;
	_root = "/tmp";
}
