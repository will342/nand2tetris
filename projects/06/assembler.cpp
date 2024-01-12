#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem; 

class Parser {
private:
    std::ifstream inputFile;
    std::ofstream outputFile;
public:
    enum commandTypes{
        A_COMMAND,
        C_COMMAND,
        L_COMMAND
    };
    Parser(const std::string& inputFilename);
    bool hasMoreCommands();
    void advance();
    commandTypes commandType();
    ~Parser();          
};


Parser::Parser(const std::string& inputFilename) {
    // Open the input file
    inputFile.open(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFilename << std::endl;
        // Handle error as needed
    }

    //Dynamically generate the output file name with ".hack" extension
    fs::path inputPath(inputFilename);
    fs::path outputPath = inputPath.replace_extension(".cln");
    outputFile.open(outputPath);
     if (!outputFile.is_open()) {
          std::cerr << "Error opening output file: " << outputPath << std::endl;
          //Handle error as needed
      }

    //Remove whitespace and comments
    std::string line;
    while(std::getline(inputFile, line)){
        // Find the position of "//" in the line
        size_t commentPos = line.find("//");

        // Remove everything after "//" if found
        if (commentPos != std::string::npos) {
            line.erase(commentPos);
        }

        // Remove whitespace from the current line
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

        // Write the modified line to the output file
        if (!line.empty()){
            outputFile << line << std::endl;
        }

    
    }
    
    //reset input file, set .cln file as new input
    inputFile.clear();
    inputFile.seekg(0);
    inputPath = outputPath;

    //create .hack ouputfile and set as output
    outputFile.close();
    outputPath = inputPath.replace_extension(".hack");
    outputFile.open(outputPath);
}

bool Parser::hasMoreCommands() {
    std::string line;
    std::getline(inputFile, line);
    bool whiteSpaceOnly = all_of(line.begin(), line.end(), [](unsigned char c) { return isspace(c); });
    return !whiteSpaceOnly;
}

void Parser::advance(){
    std::string command;
    std::getline(inputFile, command);
 }

Parser::commandTypes Parser::commandType(){
    std::string line;
    std::getline(inputFile, line);
    if (line[0] == '@'){
        return A_COMMAND;
    }
    else if (line.find('=') != std::string::npos || line.find(';') != std::string::npos){
        return C_COMMAND;
    }
    return L_COMMAND;
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
    std::string inputFileName = "test.asm";
    Parser parser(inputFileName);
    Parser::commandTypes test = parser.commandType();
    std::cout<<test; 

    return 0; // Exit with success


}