#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>


enum header {
	METHOD,
	PATH,
	VERSION
};

std::vector<std::string> parse(char* data) {
	std::stringbuf buffer(data, std::ios::in);
	std::istream stream(&buffer);

	std::vector<std::string> h;
	std::string word;

	for (int i = 0; i < 3; i++) {
		stream >> word;
		h.push_back(word);
	}

	return h;
}