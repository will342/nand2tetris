#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <bitset>

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

class Code {
private:
    Parser& parser;
public:
    std::string test2;
    Code(Parser& p);
    std::bitset<3> dest();
    std::bitset<7> comp();
    std::bitset<3> jump();
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
    return !command.empty();
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
        std::string dest = command.substr(0,command.find('='));
        std::cout<<"dest function "<<dest<<std::endl;
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
    Parser::commandTypes commandType = parser.commandType();
    if (commandType == Parser::A_COMMAND){
        int decimal = std::stoi(parser.symbol());
        std::bitset<16> binaryOut(decimal);
        parser.outputFile << binaryOut << std::endl;
    }
    else if (commandType == Parser::C_COMMAND){
        std::bitset<3> start(7);
        std::bitset<3> dest = Code::dest();
        std::bitset<7> comp = Code::comp();
        std::bitset<3> jump = Code::jump();
        if (jump !=0b000){
            dest = 0b000;
        }
        std::cout<<"dest = "<<dest<<std::endl<<"comp = "<<comp<<std::endl<<"jump = "<<jump<<std::endl;
        std::cout<<std::endl<<"full instruction"<<std::endl<<start<<comp<<dest<<jump<<std::endl;  
        parser.outputFile <<start<<comp<<dest<<jump<<std::endl;
    }
    std::cout<<"code: A or C command not found"<<std::endl;
}

std::bitset<3> Code::dest(){
    std::string dest = parser.dest();
    std::cout<<"code dest "<<dest<<std::endl;
    if (dest == ""){
        std::bitset<3> output(0);
        return output;
    }
    if (dest == "M"){
        std::bitset<3> output(1);
        return output;
    }
    if (dest == "D"){
        std::bitset<3> output(2);
        return output;
    }
    if (dest == "DM" || dest == "MD"){
        std::bitset<3> output(3);
        return output;
    }
    if (dest == "A"){
        std::bitset<3> output(4);
        return output;
    }
    if (dest == "AM"){
        std::bitset<3> output(5);
        return output;
    }
    if (dest == "AD"){
        std::bitset<3> output(6);
        return output;
    }
    if (dest == "ADM" || dest == "AMD"){
        std::bitset<3> output(7);
        return output;
    }
    std::cout<<"dest not recognized in code module"<<std::endl;
    return 0;
}

std::bitset<7> Code::comp(){
    std::string comp = parser.comp();
    if (comp == "0"){
        std::bitset<7> output(0b0101010);
        return output;
    }

    if (comp == "1"){
        std::bitset<7> output(0b0111111);
        return output;
    }

    if (comp == "-1"){
        std::bitset<7> output(0b0111010);
        return output;
    }

    if (comp == "D"){
        std::bitset<7> output(0b0001100);
        return output;
    }

    if (comp == "A"){
        std::bitset<7> output(0b0110000);
        return output;
    }

    if (comp == "!D"){
        std::bitset<7> output(0b0001101);
        return output;
    }

    if (comp == "!A"){
        std::bitset<7> output(0b0110001);
        return output;
    }

    if (comp == "-D"){
        std::bitset<7> output(0b0001111);
        return output;
    }

    if (comp == "-A"){
        std::bitset<7> output(0b0110011);
        return output;
    }

    if (comp == "D+1"){
        std::bitset<7> output(0b0011111);
        return output;
    }

    if (comp == "A+1"){
        std::bitset<7> output(0b0110111);
        return output;
    }

    if (comp == "D-1"){
        std::bitset<7> output(0b0001110);
        return output;
    }

    if (comp == "A-1"){
        std::bitset<7> output(0b0110010);
        return output;
    }

    if (comp == "D+A"){
        std::bitset<7> output(0b0000010);
        return output;
    }

    if (comp == "D-A"){
        std::bitset<7> output(0b0010011);
        return output;
    }

    if (comp == "A-D"){
        std::bitset<7> output(0b0000111);
        return output;
    }

    if (comp == "D&A"){
        std::bitset<7> output(0b0000000);
        return output;
    }

    if (comp == "D|A"){
        std::bitset<7> output(0b0010101);
        return output;
    }

    if (comp == "M"){
        std::bitset<7> output(0b1110000);
        return output;
    }

    if (comp == "!M"){
        std::bitset<7> output(0b1110001);
        return output;
    }

    if (comp == "-M"){
        std::bitset<7> output(0b1110011);
        return output;
    }

    if (comp == "M+1"){
        std::bitset<7> output(0b1110111);
        return output;
    }

    if (comp == "M-1"){
        std::bitset<7> output(0b1110010);
        return output;
    }

    if (comp == "D+M"){
        std::bitset<7> output(0b1000010);
        return output;
    }

    if (comp == "D-M"){
        std::bitset<7> output(0b1010011);
        return output;
    }

    if (comp == "M-D"){
        std::bitset<7> output(0b1000111);
        return output;
    }

    if (comp == "D&M"){
        std::bitset<7> output(0b1000000);
        return output;
    }

    if (comp == "D|M"){
        std::bitset<7> output(0b1010101);
        return output;
    }

    std::cout<<"comp not recognized in code module";
    return 0;
}

std::bitset<3> Code::jump(){
    std::string jump = parser.jump();
    std::cout<< jump << std::endl;
    if (jump.empty()){
        std::bitset<3> output(0);
        return output;
    }

    if (jump == "JGT"){
        std::bitset<3> output(1);
        return output;
    }

    if (jump == "JEQ"){
        std::bitset<3> output(2);
        return output;
    }

    if (jump == "JGE"){
        std::bitset<3> output(3);
        return output;
    }

    if (jump == "JLT"){
        std::bitset<3> output(4);
        return output;
    }

    if (jump == "JNE"){
        std::bitset<3> output(5);
        return output;
    }

    if (jump == "JLE"){
        std::bitset<3> output(6);
        return output;
    }

    if (jump == "JMP"){
        std::bitset<3> output(7);
        return output;
    }

    std::cout<<"jump not found in code module"<<std::endl;
    return 0;
}


int main() {
    std::string inputFileName = "PongL.asm";
    Parser parser(inputFileName);
    bool hasMoreCommands = true;

    while(hasMoreCommands){
        parser.advance();
        Code code(parser);
        hasMoreCommands = parser.hasMoreCommands();
    }
    
    return 0; // Exit with success
}