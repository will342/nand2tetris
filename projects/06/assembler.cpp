#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem; 

class Parser {
private:
    ifstream inputFile;
    ofstream outputFile;
public:
    enum commandType{
        A_COMMAND,
        C_COMMAND,
        L_COMMAND,
        Unknown
    }; //Ending point, I have not written the function to do the classification
    Parser(const string& inputFilename);
    bool hasMoreCommands();
    void advance();
    ~Parser();          
};

Parser::Parser(const string& inputFilename) {
    // Open the input file
    inputFile.open(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFilename << std::endl;
        // Handle error as needed
    }

    // Dynamically generate the output file name with ".hack" extension
    fs::path inputPath(inputFilename);
    fs::path outputPath = inputPath.replace_extension(".hack");
    outputFile.open(outputPath);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file: " << outputPath << endl;
        // Handle error as needed
    }
}

bool Parser::hasMoreCommands() {
    string line;
    getline(inputFile, line);
    bool whiteSpaceOnly = all_of(line.begin(), line.end(), [](unsigned char c) { return isspace(c); });
    return !whiteSpaceOnly;
}

void Parser::advance(){
    string command;
    getline(inputFile, command);
 }

// Destructor implementation
Parser::~Parser() {
    // Close the input file
    if (inputFile.is_open()) {
        inputFile.close();
    }

    // Close the output file
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

int main() {
    string inputFileName = "test.asm";
    Parser parser(inputFileName);

    //string test2 = parser.advance();

    //cout << test1 <<endl << test2 <<endl;
    return 0; // Exit with success


}