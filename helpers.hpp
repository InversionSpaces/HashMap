#include <vector>
#include <string>
#include <fstream>

std::vector<std::string> load_lines(const char* filename) {
	std::ifstream input(filename);

	std::vector<std::string> retval;
	for (	retval.emplace_back(); 
		std::getline(input, retval.back()); 
		retval.emplace_back());
	retval.pop_back();

	return retval;
}
