#include "../../includes/utils.hpp"

void	utils::trim(std::string &str, const char* toFind)
{
	str.erase(str.find_last_not_of(toFind) + 1);
	if (str.find_first_not_of(toFind) != std::string::npos)
		str.erase(0, str.find_first_not_of(toFind));
}
