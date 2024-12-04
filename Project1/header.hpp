#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>


enum header {
	method,
	path,
	version,
	connection_type
};

std::vector<std::string> parse(char* data) {
	std::stringbuf buffer(data, std::ios::in);
	std::istream stream(&buffer);

	std::vector<std::string> h;
	std::string word;
	while (stream >> word) {
		h.push_back(word);
	}

	return h;
}