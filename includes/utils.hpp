#pragma once

#include <iostream>
#include <vector>
#include <sstream>

std::vector<std::string> split(std::string str, char c);

template<typename T>
std::string	toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}
