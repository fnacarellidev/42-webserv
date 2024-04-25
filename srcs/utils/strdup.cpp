#include "../../includes/utils.hpp"

char*	utils::strdup(std::string str) {
	char* dup = (char *) std::calloc(str.size() + 1, 1);

	for (size_t i = 0; i < str.size(); ++i)
		dup[i] = str[i];

	return dup;
}
