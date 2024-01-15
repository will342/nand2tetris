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
    std::string command;
    enum commandTypes{
        A_COMMAND,
        C_COMMAND,
        L_COMMAND
    };
    Parser(const std::string& inputFilename);
    bool hasMoreCommands();
    void advance();
    commandTypes commandType();
    std::string symbol();
    std::string dest();
    std::string comp();
    std::string jump();
    ~Parser();          
};

class Code {
private:
    Parser& parser;
public:
    std::string test2;
    Code(Parser& p);
    std::string dest();
    std::string comp();
    std::string jump();
    //~Code();
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
    inputFile.close();
    inputFile.open(outputPath);
    //create .hack ouputfile and set as output
    outputFile.close();
    outputPath = inputPath.replace_extension(".hack");
    outputFile.open(outputPath);
}

bool Parser::hasMoreCommands() {
    bool whiteSpaceOnly = all_of(command.begin(), command.end(), [](unsigned char c) { return isspace(c); });
    return !whiteSpaceOnly;
}

void Parser::advance(){
    std::getline(inputFile, command);
 }

Parser::commandTypes Parser::commandType(){
    if (command[0] == '@'){
        return A_COMMAND;
    }
    else if (command.find('=') != std::string::npos || command.find(';') != std::string::npos){
        return C_COMMAND;
    }
    return L_COMMAND;
}

std::string Parser::symbol(){
    Parser::commandTypes commandType = Parser::commandType();
    if (commandType == A_COMMAND){
        std::string decimal = command.substr(1);
        return decimal;
    }
    else if (commandType == L_COMMAND){
        return command;
    }
    return "Not an A or L command";
}

std::string Parser::dest(){
    Parser::commandTypes commandType = Parser::commandType();
    if (commandType = C_COMMAND){
        std::string dest = command.substr(0,1);
        return dest;
    }
    return "Not a C command";
}

std::string Parser::comp(){
    Parser::commandTypes commandType = Parser::commandType();
    if (commandType = C_COMMAND){
        std::string comp = command.substr(command.find('=') + 1, command.find(';'));
        return comp;
    }
    return "Not a C command";
}

std::string Parser::jump(){
    //Parser::commandTypes commandType = Parser::commandType();
    if (true){
        std::string comp = command.substr(command.find(';')+1);
        return comp;
    }
    return "";
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

Code::Code(Parser& p) : parser(p) {
    std::cout<<parser.jump();
}

int main() {
    std::string inputFileName = "test.asm";
    Parser parser(inputFileName);
    parser.advance();
    std::string test = parser.jump();
    std::cout<<test;
    Code code(parser);

    return 0; // Exit with success


}