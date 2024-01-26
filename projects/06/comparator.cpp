#include <iostream>
#include <fstream>
#include <sstream>

bool areFilesEqual(const std::string& file1, const std::string& file2) {
    std::ifstream stream1(file1, std::ios::binary);
    std::ifstream stream2(file2, std::ios::binary);

    if (!stream1.is_open() || !stream2.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return false;
    }

    // Read the content of each file as strings
    std::stringstream content1, content2;
    content1 << stream1.rdbuf();
    content2 << stream2.rdbuf();

    // Compare the content strings
    return content1.str() == content2.str();
}

int main() {
    std::string file1 = "Pong.hack";
    std::string file2 = "Pong_truth.hack";

    if (areFilesEqual(file1, file2)) {
        std::cout << "The contents of the files are identical." << std::endl;
    } else {
        std::cout << "The contents of the files are not identical." << std::endl;
    }

    return 0;
}

  