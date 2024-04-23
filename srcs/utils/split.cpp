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

std::vector<std::string> split(std::string str, std::string c) {
	std::vector<std::string>	splited;
	std::stringstream ss(str);
	size_t	pos = 0;
	size_t	prev = 0;
	while ((pos = str.find(c, prev)) != std::string::npos) {
		splited.push_back(str.substr(prev, pos - prev));
		prev = pos + c.size();
	}
	if (prev != str.length())
		splited.push_back(str.substr(prev));
	return (splited);
}
