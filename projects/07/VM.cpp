#include <iostream>
#include <string>
#include <fstream>
#include <vector>

int main() {

	// Testing
	std::cout << "Hello, VM!" << std::endl;
	std::ifstream inputFile("MemoryAccess/BasicTest/BasicTest.vm");

	if (inputFile.is_open()) {
		std::string line;
		while (getline(inputFile, line)) {
			std::cout << line << std::endl;
		}

	}
	else {
		std::cout << "Unable to open file" << std::endl;
	}

	/*
	approach
	-open file
	-if the line is not a comment or blank, load it into a vector of strings
	-need to decide if this is in the class constructor or a member function
	*/
	return 0;
}