#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

class Parser {
	public:
	std::vector<std::string> lines;
	std::string currentCommand;
	std::vector<std::string> commandTokens;

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

	std::unordered_map<std::string, commandTypes> commandDict = {
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
		{"label", C_LABEL},
		{"goto", C_GOTO},
		{"if-goto", C_IF},
		{"function", C_FUNCTION},
		{"return", C_RETURN},
		{"call", C_CALL}
    };


	Parser(const std::string& inputFilename);
	commandTypes commandType();
	void printLines();
	bool hasMoreCommands();
	void advance();
	std::string arg1();
	const int arg2();
	bool isPushPopFuncCall(commandTypes cmd);
};

class Writer {
	public:
	Writer(const std::string& inputFilename); 
	void writeArithmetic(const std::string& command); 
	void writePushPop(Parser::commandTypes command, const std::string& segment, int index); 
	~Writer(); 
};

Writer::Writer(const std::string& inputFilename) {
	// opens the output file/stream and gets ready to write
}

void Writer::writeArithmetic(const std::string& command) {
	// writes the assembly code that is the translation of the given arithmetic command
}

void Writer::writePushPop(Parser::commandTypes command, const std::string& segment, int index) {
	// writes the assembly code that is the translation of the given push or pop command
}

Writer::~Writer() {
	//closes the output file
}

Parser::Parser(const std::string& inputFilename) {
	std::ifstream inputFile(inputFilename);
	if (inputFile.is_open()) {
		std::string line;
		while (getline(inputFile, line)) {
			// Find the position of "//" in the line
        	size_t commentPos = line.find("//");

        	// Remove everything after "//" if found
        	if (commentPos != std::string::npos) {
            line.erase(commentPos);
        	}

			// Trim whitespaces
       		auto notSpace = [](int ch){ return !std::isspace(ch); };
			line.erase(line.begin(), std::find_if(line.begin(), line.end(), notSpace));
			line.erase(std::find_if(line.rbegin(), line.rend(), notSpace).base(), line.end());

			// Add line to lines vector if not empty
			if(!line.empty()){
				lines.push_back(line);
			}
		}
	}
	else {
		std::cout << "Parser constructor was unable to open file" << std::endl;
	}
}

void Parser::printLines() {
	for (const auto& line : lines) {
		std::cout << line << std::endl;
	}
}

bool Parser::hasMoreCommands() {
	return !lines.empty();
}

void Parser::advance() {
	//advance and split the line into a vector of command tokens
	commandTokens.clear();

	if (hasMoreCommands()) {
		currentCommand = lines.front();
		std::istringstream iss(currentCommand);
   		std::string token;
    	while (iss >> token) {
        	commandTokens.push_back(token);
    	}

		lines.erase(lines.begin());
	}
}

Parser::commandTypes Parser::commandType() {
	auto it = commandDict.find(commandTokens.front());
	if (it != commandDict.end()) {
		return it->second;
	}
	else return UNKNOWN;
}

std::string Parser::arg1() {
	if (commandType() == C_ARITHMETIC) {
		return commandTokens.front();
	} else if (commandTokens.size() > 1) {
		return commandTokens[1];
	} else if (commandType() == C_RETURN) {
		return "invalid arg1() call with C_RETURN command";
	}
	return "";
}

bool Parser::isPushPopFuncCall(Parser::commandTypes cmd) {
    return cmd == Parser::C_PUSH
        || cmd == Parser::C_POP
        || cmd == Parser::C_FUNCTION
        || cmd == Parser::C_CALL;
}

const int Parser::arg2() {
	if (isPushPopFuncCall(commandType()) && commandTokens.size() > 2) {
		return std::stoi(commandTokens[2]);
	}
	return -1;
}


int main() {
	const std::string& vmFile = "MemoryAccess/BasicTest/BasicTest.vm";

	std::cout << "Initializing parser with file " << vmFile << "\n\n";
	Parser parser(vmFile);

	while (parser.hasMoreCommands()) {
		parser.advance();
		std::cout << "Current command : " << parser.currentCommand << std::endl;
		std::cout << "Command type : " << parser.commandType() << std::endl;
		std::cout << "arg1 : " << parser.arg1() << std::endl;
		if(parser.isPushPopFuncCall(parser.commandType())) {
			std::cout << "arg2 : " << parser.arg2() << std::endl;
		}
	}

	Writer writer(vmFile);

	/*
	stage 1 - implement the 9 stack arithmetic and logical commands as well as the push constant x command
	(generic push command for the special case where the first argument is constant and the second argument is some decimal constant)

	stage 2 - include a full implementation of push and pop commands handling all eight memory segments
		2a - constant segment (already handled)
		2b - handle the segments local, argument, this, and that
		2c - pointer and temp segments - in particular allowing mod of the basses of this and that segments
		2d - static segments
	*/

	return 0;
}