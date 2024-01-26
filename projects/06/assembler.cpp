#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <bitset>
#include <unordered_map>
#include <map>

namespace fs = std::filesystem; 

class Parser {
private:
public:
    std::ifstream inputFile;
    std::ofstream outputFile;
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

class SymbolTable {
    private:
    public:
        std::unordered_map<std::string, int> symbolTable;
        SymbolTable();
        void addEntry(std::string symnbol, int address);
        bool contains(std::string symbol);
        int getAddress(std::string symbol);
};

class Code {
private:
    Parser& parser;
    SymbolTable& table;
    int& RAM_address;
public:
    Code(Parser& p, SymbolTable& t, int& address);
    std::bitset<3> dest();
    std::bitset<7> comp();
    std::bitset<3> jump();
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
    return !command.empty();
}

void Parser::advance(){
    std::getline(inputFile, command);
 }

Parser::commandTypes Parser::commandType(){
    //check command that first char is @ and second char is a number
    if (command[0] == '@' && std::isdigit(static_cast<unsigned char>(command[1]))){
        return A_COMMAND;
    }
    //check that command either has = or ;
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
        std::string dest = command.substr(0,command.find('='));
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

Code::Code(Parser& p, SymbolTable& t, int& address) : parser(p), table(t), RAM_address(address) {
    Parser::commandTypes commandType = parser.commandType();
    //write an A command
    if (commandType == Parser::A_COMMAND){
        int decimal = std::stoi(parser.symbol());
        std::bitset<16> binaryOut(decimal);
        parser.outputFile << binaryOut << std::endl;
    }
    
    //write a C command
    else if (commandType == Parser::C_COMMAND){
        std::bitset<3> start(7);
        std::bitset<3> dest = Code::dest();
        std::bitset<7> comp = Code::comp();
        std::bitset<3> jump = Code::jump();
        if (jump !=0b000){
            dest = 0b000;
        }
        parser.outputFile <<start<<comp<<dest<<jump<<std::endl;
        }
        
    //write an L command, generate new symbol table entry if needed
    else if (commandType == Parser::L_COMMAND && parser.command[0] == '@'){
        std::string commandSub = parser.command.substr(1);
        if (table.contains(commandSub)){
            int address = table.getAddress(commandSub);
            std::bitset<16> binaryOut(address);
            parser.outputFile << binaryOut << std::endl;
            }

        else{
            table.addEntry(commandSub, RAM_address);
            std::bitset<16> binaryOut(RAM_address);
            parser.outputFile << binaryOut << std::endl;
            RAM_address +=1;
        }
    }
}

std::bitset<3> Code::dest(){
    std::string dest = parser.dest();
    const std::map<std::string, std::bitset<3>> destMap = {
        {"",0},
        {"M",1},
        {"D",2},
        {"DM",3},
        {"MD",3},
        {"A",4},
        {"AM",5},
        {"AD",6},
        {"ADM",7},
        {"AMD",7}
    };

    auto it = destMap.find(dest);
    if (it != destMap.end()){
        return it->second;
    }

    return 0;
}

std::bitset<7> Code::comp(){
    std::string comp = parser.comp();
    const std::map<std::string, std::bitset<7>> compMap = {
        {"0",0b0101010},
        {"1",0b0111111},
        {"-1",0b0111010},
        {"D",0b0001100},
        {"A",0b0110000},
        {"!D",0b0001101},
        {"!A",0b0110001},
        {"-D",0b0001111},
        {"-A",0b0110011},
        {"D+1",0b0011111},
        {"A+1",0b0110111},
        {"D-1",0b0001110},
        {"A-1",0b0110010},
        {"D+A",0b0000010},
        {"D-A",0b0010011},
        {"A-D",0b0000111},
        {"D&A",0b0000000},
        {"D|A",0b0010101},
        {"M",0b1110000},
        {"!M",0b1110001},
        {"-M",0b1110011},
        {"M+1",0b1110111},
        {"M-1",0b1110010},
        {"D+M",0b1000010},
        {"D-M",0b1010011},
        {"M-D",0b1000111},
        {"D&M",0b1000000},
        {"D|M",0b1010101}
    };

    auto it = compMap.find(comp);
    if (it != compMap.end()){
        return it->second;
    }

    return 0;
}

std::bitset<3> Code::jump(){
    std::string jump = parser.jump();
    // Mapping jump strings to corresponding bitset values
    const std::map<std::string, std::bitset<3>> jumpMap = {
        {"", 0},   // Empty string
        {"JGT", 1},
        {"JEQ", 2},
        {"JGE", 3},
        {"JLT", 4},
        {"JNE", 5},
        {"JLE", 6},
        {"JMP", 7}
    };

    // Using the map to get the corresponding bitset value
    auto it = jumpMap.find(jump);
    if (it != jumpMap.end()) {
        return it->second;
    }

    return 0;
}

SymbolTable::SymbolTable(){
    //create R0-R15 entries
    for (int i=0; i<16; i++){
        std::string key = "R" + std::to_string(i);
        symbolTable[key] = i;
    }

    //create other pre-defined symbols
    symbolTable["SP"] = 0;
    symbolTable["LCL"] = 1;
    symbolTable["ARG"] = 2;
    symbolTable["THIS"] = 3;
    symbolTable["THAT"] = 4;
    symbolTable["SCREEN"] = 16384;
    symbolTable["KBD"] = 24576;
}

void SymbolTable::addEntry(std::string symbol, int address){
    SymbolTable::symbolTable[symbol] = address;
}

bool SymbolTable::contains(std::string symbol){
    auto it = SymbolTable::symbolTable.find(symbol);
    if (it !=SymbolTable::symbolTable.end()){
        return true;
    }

    return false;
}

int SymbolTable::getAddress(std::string symbol){
    return SymbolTable::symbolTable.find(symbol)->second;
}

int main() {
    std::string inputFileName = "Pong.asm";
    Parser parser(inputFileName);
    bool hasMoreCommands = true;
    SymbolTable table;
    int ROM_address = 0;
    int RAM_address = 16;

    //pass 1: setup up (xxx) pseudocommand
    while(hasMoreCommands){
        parser.advance();
        if (parser.commandType() == Parser::L_COMMAND && parser.command[0] == '('){
            std::string subCommand = parser.command.substr(1, parser.command.length() - 2);
            table.addEntry(subCommand, ROM_address);
            ROM_address -=1;
        }
        ROM_address += 1;
        hasMoreCommands = parser.hasMoreCommands();
    }

    //reset for next pass
    hasMoreCommands = true;
    parser.inputFile.clear();
    parser.inputFile.seekg(0);

    //pass 2: other symbols and write
    while(hasMoreCommands){
        parser.advance();
        Code code(parser, table, RAM_address);
        hasMoreCommands = parser.hasMoreCommands();
    }

    //symbol table test output
    for (const auto& pair : table.symbolTable) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }

   
    return 0; // Exit with success
}