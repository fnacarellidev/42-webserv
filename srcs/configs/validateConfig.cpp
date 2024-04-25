
#include "../../includes/WebServer.hpp"
#include "../../includes/utils.hpp"

static bool	validateCGIConfig(std::string& cgi) {
	std::vector<std::string> types = utils::split(cgi, ',');

	if (types.size() > 3)
		return true;
	for (std::vector<std::string>::iterator it = types.begin(); it != types.end(); it++)
		if (*it != ".php" && *it != ".py")
			return true;
	return false;
}

static bool	validateRedirectConfig(std::string& redirect) {
	std::vector<std::string> values = utils::split(redirect, '=');

	return (values.size() != 2 || values.at(1)[0] != '/' || values.at(0)[0] != '/');
}

static bool	validateMethodsConfig(std::string& methods) {
	std::vector<std::string>	values = utils::split(methods, ',');

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

static bool	validateErrorConfig(std::string& errors) {
	std::vector<std::string> pairs = utils::split(errors, ',');
	
	for (std::vector<std::string>::iterator it = pairs.begin(); it != pairs.end(); it++) {
		std::vector<std::string> values = utils::split(*it, '=');

		if (values.size() != 2 || values[0].size() != 3)
			return true;

		long int	code = std::strtol(values[0].c_str(), NULL, 10);

		if (code < 100 || code > 599)
			return true;
	}
	return false;
}

static bool	validateHostConfig(std::string& ip) {
	if (ip.find_first_not_of("0123456789.") != std::string::npos)
		return true;

	std::vector<std::string> bits = utils::split(ip, '.');

	if (bits.size() != 4)
		return true;
	for (std::vector<std::string>::iterator it = bits.begin(); it != bits.end(); it++) {
		if (it->size() > 3)
			return true;

		long int nbr = std::strtol(it->c_str(), NULL, 10);

		if (nbr < 0 || nbr > 255)
			return true;
	}
	return false;
}

static bool	validateLimitConfig(std::string& limit) {
	if (limit.find_first_not_of("0123456789KkBbMmGg") != std::string::npos)
		return true;
	errno = 0;

	char*	rest = NULL;
	size_t	nbr = std::strtoull(limit.c_str(), &rest, 10);
	
	if (errno == ERANGE)
		return true;

	std::string byteType(rest[0] == 0 ? "b" : rest);

	if (byteType.size() > 2)
		return true;
	for (std::string::iterator it = byteType.begin(); it != byteType.end(); it++)
		*it = std::tolower(*it);
	if (byteType != "b" && byteType != "kb" && byteType != "mb" && byteType != "gb")
		return true;
	return ((byteType == "gb" && nbr > (size_t)GIGA_LIMIT) || (byteType == "mb" && nbr > (size_t)MEGA_LIMIT) ||
	(byteType == "kb" && nbr > (size_t)KILO_LIMIT));
}

static bool	validatePortConfig(std::string& port) {
	if (port.find_first_not_of("0123456789") != std::string::npos)
		return true;
	errno = 0;

	size_t	nbr = std::strtoull(port.c_str(), NULL, 10);

	return (errno == ERANGE || nbr > std::numeric_limits<unsigned short int>::max());
}

void	checkInsideRoute(std::ifstream& file, std::string& line)
throw(std::runtime_error) {
	std::map<std::string, Route::Keywords> routeMap(buildRouteMap());
	bool routeBrackets = false, error = false;

	while (!file.eof()) {
		std::vector<std::string> splited = utils::split(line, ' ');

		if (!splited.empty()) {
			std::map<std::string, Route::Keywords>::iterator found = routeMap.find(splited[0]);

			if (splited.size() == 1 && splited[0] == "}" && routeBrackets)
				return ;
			if (splited.size() != 2 || found == routeMap.end())
				throw std::runtime_error("");

			switch (found->second) {
				case Route::ROUTE:
					if (splited[1] != "{" || routeBrackets)
						throw std::runtime_error("");
					routeBrackets = !routeBrackets;
					break;
				default:
					if (splited[1].find_first_of(";") == std::string::npos ||
					splited[1].find_last_of(';') != splited[1].find_first_of(';') ||
					*(splited[1].end() - 1) != ';')
						throw std::runtime_error("");
					splited[1].erase(splited[1].end() - 1);
					switch (found->second) {
						case Route::METHODS:
							error = validateMethodsConfig(splited[1]);
							break;
						case Route::REDIRECT:
							error = validateRedirectConfig(splited[1]);
							break;
						case Route::ROOT:
							error = splited.at(1)[0] != '/';
							break;
						case Route::LISTING:
							error = (splited[1] != "on" && splited[1] != "off");
							break;
						case Route::PATH:
							error = (*splited[1].begin() != '/');
							break;
						case Route::CGI:
							error = validateCGIConfig(splited[1]);
							break;
						default: ;
					}
			}
			if (error)
				throw std::runtime_error("");
		}
		line.clear();
		std::getline(file, line);
		utils::trim(line, "\t \n");
	}
}

bool	invalidServerInputs(std::ifstream& file,
std::string& line,
bool* serverBrackets,
std::map<std::string, Server::Keywords>& serverMap) {
	bool	error = false;

	while (!file.eof()) {
		std::vector<std::string> splited = utils::split(line, ' ');

		if (!splited.empty()) {
			std::map<std::string, Server::Keywords>::iterator found = serverMap.find(splited[0]);

			if (splited.size() == 1 && splited[0] == "}" && *serverBrackets) {
				*serverBrackets = !*serverBrackets;
				return false;
			}
			if (splited.size() != 2 || found == serverMap.end())
				return true;

			switch (found->second) {
				case Server::SERVER:
					if (splited[1] != "{" || *serverBrackets)
						return true;
					*serverBrackets = !*serverBrackets;
					break;
				case Server::ROUTE:
					if (splited[1] != "{")
						return true;
					try {
						checkInsideRoute(file, line);
					} catch (std::exception& e) {
						return true;
					}
					break;
				default:
					if (splited[1].find_first_of(";") == std::string::npos ||
					splited[1].find_last_of(';') != splited[1].find_first_of(';') ||
					*(splited[1].end() - 1) != ';')
						return true;
					splited[1].erase(splited[1].end() - 1);
					switch (found->second) {
						case Server::ERROR:
							error = validateErrorConfig(splited[1]);
							break;
						case Server::HOST:
							error = validateHostConfig(splited[1]);
							break;
						case Server::LIMIT:
							error = validateLimitConfig(splited[1]);
							break;
						case Server::PORT:
							error = validatePortConfig(splited[1]);
							break;
						case Server::ROOT:
							error = splited.at(1)[0] != '/';
							break;
						default: ;
					}
			}
			if (error)
				return error;
		}
		line.clear();
		std::getline(file, line);
		utils::trim(line, "\t \n");
	}
	return false;
}
