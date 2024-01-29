#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>

namespace fs = std::filesystem; 

class Parser {
private:
public:
    std::ifstream inputFile;
    std::ofstream outputFile;
    std::string command;
    enum commandTypes{
        C_ARITHMETIC,
        C_PUSH,
        C_POP,
        C_LABEL,
        C_GOTO,
        C_IF,
        C_FUNCTION,
        C_RETURN,
        C_CALL,
        UNKNOWN
    };
    Parser(const std::string& inputFilename);
    bool hasMoreCommands();
    void advance();
    commandTypes commandType();
    std::string arg1();
    int arg2();
    ~Parser();          
};

class CodeWriter {
    private:
    public:
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
    outputPath = inputPath.replace_extension(".asm");
    outputFile.open(outputPath);
}

bool Parser::hasMoreCommands() {
    return !command.empty();
}

void Parser::advance(){
    std::getline(inputFile, command);
}

Parser::commandTypes Parser::commandType(){
    std::istringstream iss(command);
    std::string firstWord;
    iss >> firstWord;

    const std::map<std::string, Parser::commandTypes> commandMap = {
        {"add", C_ARITHMETIC},
        {"sub", C_ARITHMETIC},
        {"neg", C_ARITHMETIC},
        {"eq", C_ARITHMETIC},
        {"gt", C_ARITHMETIC},
        {"lt", C_ARITHMETIC},
        {"and", C_ARITHMETIC},
        {"or", C_ARITHMETIC},
        {"not", C_ARITHMETIC},
        {"push", C_PUSH},
        {"pop", C_POP},
    };
    auto it = commandMap.find(firstWord);
    if (it != commandMap.end()){
        return it->second;
    }
    return UNKNOWN;
}

std::string Parser::arg1(){
    if (commandType() == C_ARITHMETIC){
        std::istringstream iss(command);
        std::string first;
        iss >> first;
        return first;
    }
    if (commandType() == C_RETURN){
        return "Incorrectly called arg1 function with C_RETURN command";
    }
    else{
        std::istringstream iss(command);
        std::string first, second;
        iss >> first >> second;
        return second;
    }
}

int Parser::arg2(){
    if (commandType() == C_PUSH || commandType() == C_POP || commandType() == C_FUNCTION || commandType() == C_CALL){
        std::istringstream iss(command);
        std::string first, second, third;
        iss >> first >> second >> third;
        return std::stoi(third);
    }
    std::cout<<"Incorrectly called arg2 function with wrong command type"<<std::endl;
    return 0;
}

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

int main(){
    Parser parser("test.vm");

    bool hasMoreCommands = true;

    while(hasMoreCommands){
        parser.advance();
        hasMoreCommands = parser.hasMoreCommands();
    }

    return 0;
}