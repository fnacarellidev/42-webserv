#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include <cctype>
#include <limits>
#include <algorithm>
#include <map>
#include <vector>
#include <exception>
#include <cerrno>

std::map<std::string, Server::Keywords>	buildServerMap()
{
	std::map<std::string, Server::Keywords> map;

	map["server"] = Server::SERVER;
	map["host"] = Server::HOST;
	map["port"] = Server::PORT;
	map["names"] = Server::NAMES;
	map["limit"] = Server::LIMIT;
	map["error"] = Server::ERROR;
	map["route"] = Server::ROUTE;

	return map;
}

std::map<std::string, Route::Keywords>	buildRouteMap()
{
	std::map<std::string, Route::Keywords> map;

	map["route"] = Route::ROUTE;
	map["index"] = Route::INDEX;
	map["redirect"] = Route::REDIRECT;
	map["root"] = Route::ROOT;
	map["methods"] = Route::METHODS;
	map["listing"] = Route::LISTING;
	map["cgi"] = Route::CGI;

	return map;
}


ServerConfig&	searchViaHost(std::string const& host, unsigned int const port, std::vector<ServerConfig>& servers) throw(ServerNotFound)
{
	std::vector<ServerConfig>::iterator end(servers.end());
	std::vector<ServerConfig>::iterator begin(servers.begin());

	while (begin != end) {
		if (begin->getHost() == host && begin->getPort() == port)
			return *begin;
		++begin;
	}
	throw ServerNotFound();
}

ServerConfig*	searchViaName(std::string const name, unsigned int const port, std::vector<ServerConfig>& servers)
{
	std::vector<ServerConfig>::iterator end(servers.end());
	std::vector<ServerConfig>::iterator begin(servers.begin());

	while (begin != end) {
		std::vector<std::string> names = begin->getNames();

		for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++) {
			if (*it == name && begin->getPort() == port)
				return &(*begin);
		}
		++begin;
	}
	return NULL;
}

bool	validateErrorConfig(std::string& errors)
{
	std::vector<std::string> pairs = split(errors, ',');
	
	for (std::vector<std::string>::iterator it = pairs.begin(); it != pairs.end(); it++) {
		std::vector<std::string> values = split(*it, '=');

		if (values.size() != 2 || values[0].size() != 3)
			return true;
		
		int	code = std::strtol(values[0].c_str(), 0, 10);

		if (code < 100 || code > 599)
			return true;
	}
	return false;
}

bool	validateHostConfig(std::string& ip)
{
	if (ip.find_first_not_of("0123456789.") != std::string::npos)
		return true;

	std::vector<std::string> bits = split(ip, '.');

	if (bits.size() != 4)
		return true;
	for (std::vector<std::string>::iterator it = bits.begin(); it != bits.end(); it++) {
		if (it->size() > 3)
			return true;
		errno = 0;

		long int nbr = std::strtol(it->c_str(), 0, 10);

		if (errno == ERANGE || (nbr < 0 || nbr > 255))
			return true;
	}
	return false;
}

bool	validateLimitConfig(std::string& limit)
{
	if (limit.find_first_of('-') != std::string::npos)
		return true;
	errno = 0;

	char*	rest = NULL;
	size_t	nbr = std::strtoull(limit.c_str(), &rest, 10); // se estourar ele seta pra SIZE_MAX
	
	if (errno == ERANGE)
		return true;

	std::string byteType(rest[0] == 0 ? "b" : rest);

	if (byteType.size() > 2)
		return true;
	for (std::string::iterator it = byteType.begin(); it != byteType.end(); it++)
		*it = std::tolower(*it);
	if (byteType != "b" && byteType != "kb" && byteType != "mb" && byteType != "gb")
		return true;
	return ((byteType == "gb" && nbr > (size_t)GIGA_LIMIT) || (byteType == "mb" && nbr > (size_t)MEGA_LIMIT) || (byteType == "kb" && nbr > (size_t)KILO_LIMIT));
}

bool	validatePortConfig(std::string& port)
{
	if (port.find_first_not_of("0123456789") != std::string::npos)
		return true;
	errno = 0;

	size_t	nbr = std::strtoull(port.c_str(), 0, 10);

	return (errno == ERANGE || nbr > std::numeric_limits<unsigned short int>::max());
}

bool	validateRedirectConfig(std::string& redirect)
{
	std::vector<std::string> values = split(redirect, '=');

	return (values.size() != 2 || values.at(1)[0] != '/' || values.at(0)[0] != '/');
}

bool	validateMethodsConfig(std::string& methods)
{
	std::vector<std::string>	values = split(methods, ',');

	if (values.size() > 3)
		return true;
	for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++) {
		for (std::string::iterator it2 = it->begin(); it2 != it->end(); it2++)
			*it2 = std::toupper(*it2);
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			return true;
	}
	return false;
}
