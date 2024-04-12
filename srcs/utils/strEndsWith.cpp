#include <iostream>

bool strEndsWith(std::string str, char c) {
	size_t last = str.size() - 1;

	return str[last] == c; 
}
