#include "../../includes/utils.hpp"
#include <sstream>

std::vector<std::string> split(std::string str, char c) {
    std::string word;
    std::stringstream ss(str);
	std::vector<std::string> splitted;

	while (std::getline(ss, word, c))
		splitted.push_back(word);

	return splitted;
}
