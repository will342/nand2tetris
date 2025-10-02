#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <filesystem>

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
	bool isPushPopFuncCall();
	bool isPushPop();
};

class Writer {
	public:
	std::ofstream outputFile;
	int labelCounter = 1;

	Writer(const std::string& inputFilename); 
	void writeArithmetic(const std::string& command); 
	void writePushPop(Parser::commandTypes command, const std::string& segment, int index); 
	void writeInit();
	void writeLabel(const std::string& label);
	void writeGoto(const std::string& label);
	void writeIf(const std::string& label);
	void writeCall(const std::string& functionName, int numArgs);
	void writeReturn();
	void writeFunction(const std::string& functionName, int numLocals);
	~Writer(); 
};

Writer::Writer(const std::string& inputFilename) {
	// opens the output file/stream and gets ready to write
	std::filesystem::path inputPath(inputFilename);
    std::filesystem::path outputPath = inputPath.replace_extension(".asm");
    outputFile.open(outputPath);
    if (!outputFile.is_open()) {
    	std::cerr << "Error opening output file: " << outputPath << std::endl;
        //Handle error as needed
    }

}

void Writer::writeArithmetic(const std::string& command) {
	// writes the assembly code that is the translation of the given arithmetic command

	if (command == "add") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "D=M\n" << "@SP\n" << "M=M-1\n"
		 			<< "A=M\n" << "D=D+M\n" << "@SP\n" << "A=M\n" << "M=D\n" << "@SP\n" << "M=M+1\n";
	}

	if (command == "sub") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "D=M\n" << "@SP\n" << "M=M-1\n"
		 			<< "A=M\n" << "D=M-D\n" << "@SP\n" << "A=M\n" << "M=D\n" << "@SP\n" << "M=M+1\n";
	}

	if (command == "neg") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "M=-M\n" << "@SP\n" << "M=M+1\n";
	}

	if (command == "eq") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "D=M\n" << "@SP\n" << "M=M-1\n"
		 			<< "A=M\n" << "D=M-D\n" << "@SP\n" << "A=M\n" <<"M=0\n" << "@EQ_TRUE" << labelCounter << "\n" << "D;JEQ\n"
					<<"@EQ_CONT" << labelCounter << "\n" << "0;JMP\n" << "(EQ_TRUE" << labelCounter << ")\n" << "@SP\n" << "A=M\n"
					<< "M=-1\n" << "(EQ_CONT" << labelCounter << ")\n" << "@SP\n" << "M=M+1\n";
		labelCounter++;
	}

	if (command == "gt") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "D=M\n" << "@SP\n" << "M=M-1\n"
					<< "A=M\n" << "D=M-D\n" << "@SP\n" << "A=M\n" <<"M=0\n"<< "@GT_TRUE" << labelCounter << "\n" << "D;JGT\n"
					<< "@GT_CONT" << labelCounter << "\n" << "0;JMP\n" << "(GT_TRUE" << labelCounter << ")\n" << "@SP\n" << "A=M\n"
					<< "M=-1\n" << "(GT_CONT" << labelCounter << ")\n" << "@SP\n" << "M=M+1\n";
		labelCounter++;
	}

	if (command == "lt") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "D=M\n" << "@SP\n" << "M=M-1\n"
					<< "A=M\n" << "D=M-D\n"<< "@SP\n" << "A=M\n" <<"M=0\n" << "@LT_TRUE" << labelCounter << "\n" << "D;JLT\n"
					<< "@LT_CONT" << labelCounter << "\n" << "0;JMP\n" << "(LT_TRUE" << labelCounter << ")\n" << "@SP\n" << "A=M\n"
					<< "M=-1\n" << "(LT_CONT" << labelCounter << ")\n" << "@SP\n" << "M=M+1\n";
		labelCounter++;
	}

	if (command == "and") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "D=M\n" << "@SP\n" << "M=M-1\n"<< "A=M\n" << "M=D&M\n" << "@SP\n" << "M=M+1\n";
	}

	if(command == "or") {
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "D=M\n" << "@SP\n" << "M=M-1\n"<< "A=M\n" << "M=D|M\n" << "@SP\n" << "M=M+1\n";
	}

	if(command == "not"){
		outputFile << "@SP\n" << "M=M-1\n" << "A=M\n" << "M=!M\n" << "@SP\n" << "M=M+1\n";
	}
}	

void Writer::writePushPop(Parser::commandTypes command, const std::string& segment, int index) {
	// writes the assembly code that is the translation of the given push or pop command

	if (command == Parser::C_PUSH) {
		if (segment == "constant"){
			outputFile << "@" << index << "\n" << "D=A\n" << "@SP\n" << "A=M\n" << "M=D\n" << "@SP\n" << "M=M+1\n";
		}

		if (segment == "local") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A" << "\n"
			<< "@LCL" << "\n"
			<< "A=M" << "\n"
			<< "A=A+D" << "\n"
			<< "D=M" << "\n"
			<< "@SP" << "\n"
			<< "A=M" << "\n"
			<< "M=D" << "\n"
			<< "@SP" << "\n"
			<< "M=M+1\n";
		}

		if (segment == "argument") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A" << "\n"
			<< "@ARG" << "\n"
			<< "A=M" << "\n"
			<< "A=A+D" << "\n"
			<< "D=M" << "\n"
			<< "@SP" << "\n"
			<< "A=M" << "\n"
			<< "M=D" << "\n"
			<< "@SP" << "\n"
			<< "M=M+1\n";
		}
		
		if (segment == "this") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A" << "\n"
			<< "@THIS" << "\n"
			<< "A=M" << "\n"
			<< "A=A+D" << "\n"
			<< "D=M" << "\n"
			<< "@SP" << "\n"
			<< "A=M" << "\n"
			<< "M=D" << "\n"
			<< "@SP" << "\n"
			<< "M=M+1\n";
		}

		if (segment == "that") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A" << "\n"
			<< "@THAT" << "\n"
			<< "A=M" << "\n"
			<< "A=A+D" << "\n"
			<< "D=M" << "\n"
			<< "@SP" << "\n"
			<< "A=M" << "\n"
			<< "M=D" << "\n"
			<< "@SP" << "\n"
			<< "M=M+1\n";
		}

		if (segment == "temp") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A" << "\n"
			<< "@5" << "\n"
			<< "A=A+D" << "\n"
			<< "D=M" << "\n"
			<< "@SP" << "\n"
			<< "A=M" << "\n"
			<< "M=D" << "\n"
			<< "@SP" << "\n"
			<< "M=M+1\n";
		}

		if (segment == "pointer") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A" << "\n"
			<< "@3" << "\n"
			<< "A=A+D" << "\n"
			<< "D=M" << "\n"
			<< "@SP" << "\n"
			<< "A=M" << "\n"
			<< "M=D" << "\n"
			<< "@SP" << "\n"
			<< "M=M+1\n";
		}

		if (segment == "static") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A" << "\n"
			<< "@16" << "\n"
			<< "A=A+D" << "\n"
			<< "D=M" << "\n"
			<< "@SP" << "\n"
			<< "A=M" << "\n"
			<< "M=D" << "\n"
			<< "@SP" << "\n"
			<< "M=M+1\n";
		}
	}	

	if (command == Parser::C_POP) {
		if (segment == "local") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A\n"
			<< "@LCL\n"
			<< "D=D+M\n"
			<< "@R13\n"
			<< "M=D\n"
			<< "@SP\n"
			<< "M=M-1\n"
			<< "A=M\n"
			<< "D=M\n"
			<< "@R13\n"
			<< "A=M\n"
			<< "M=D\n";
			}
			
		if (segment == "argument") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A\n"
			<< "@ARG\n"
			<< "D=D+M\n"
			<< "@R13\n"
			<< "M=D\n"
			<< "@SP\n"
			<< "M=M-1\n"
			<< "A=M\n"
			<< "D=M\n"
			<< "@R13\n"
			<< "A=M\n"
			<< "M=D\n";
			}

		if (segment == "this") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A\n"
			<< "@THIS\n"
			<< "D=D+M\n"
			<< "@R13\n"
			<< "M=D\n"
			<< "@SP\n"
			<< "M=M-1\n"
			<< "A=M\n"
			<< "D=M\n"
			<< "@R13\n"
			<< "A=M\n"
			<< "M=D\n";
			}

		if (segment == "that") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A\n"
			<< "@THAT\n"
			<< "D=D+M\n"
			<< "@R13\n"
			<< "M=D\n"
			<< "@SP\n"
			<< "M=M-1\n"
			<< "A=M\n"
			<< "D=M\n"
			<< "@R13\n"
			<< "A=M\n"
			<< "M=D\n";
			}

		if (segment == "temp") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A\n"
			<< "@5\n"
			<< "D=D+A\n"
			<< "@R13\n"
			<< "M=D\n"
			<< "@SP\n"
			<< "M=M-1\n"
			<< "A=M\n"
			<< "D=M\n"
			<< "@R13\n"
			<< "A=M\n"
			<< "M=D\n";
			}

		if (segment == "pointer") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A\n"
			<< "@3\n"
			<< "D=D+A\n"
			<< "@R13\n"
			<< "M=D\n"
			<< "@SP\n"
			<< "M=M-1\n"
			<< "A=M\n"
			<< "D=M\n"
			<< "@R13\n"
			<< "A=M\n"
			<< "M=D\n";
			}

		if (segment == "static") {
			outputFile
			<< "@" << index << "\n"
			<< "D=A\n"
			<< "@16\n"
			<< "D=D+A\n"
			<< "@R13\n"
			<< "M=D\n"
			<< "@SP\n"
			<< "M=M-1\n"
			<< "A=M\n"
			<< "D=M\n"
			<< "@R13\n"
			<< "A=M\n"
			<< "M=D\n";
			}
	}
}

void Writer::writeInit() {
	//writes the assembly code that effects the VM initialization, also called bootstrap code
	//bootstrap code must be placed at the beginning of the output file
	//bootstrap code must call Sys.init function
}

void Writer::writeLabel(const std::string& label) {
	outputFile <<"("<<label<<")\n";
}

void Writer::writeGoto(const std::string& label) {
	outputFile <<"@"<<label<<"\n"
			   <<"0;JMP"<<"\n";
}

void Writer::writeIf(const std::string& label) {
	outputFile <<"@SP"<<"\n"
	           <<"M=M-1"<<"\n"
			   <<"A=M"<<"\n"
			   <<"D=M"<<"\n"
			   <<"@"<<label<<"\n"
			   <<"D;JNE"<<"\n";
}

void Writer::writeCall(const std::string& functionName, int numArgs) {
	//writes assembly code that effects the call command
}

void Writer::writeReturn() {
	//writes assembly code that effects the return command

	
}

void Writer::writeFunction(const std::string& functionName, int numLocals) {
	//writes assembly code that effects the function command f k
	//declare a label for function entry f
	//k = num of local variables
	//initialize all local variables to 0

	//set sp to match LCL
	//use sp to zero out k local variables
	//write instructions as usual

	outputFile <<"("<<functionName<<")\n" //write label
			   <<"@0\n"	//set 0 for 2 local variables	
			   <<"A=D\n"
			   <<"@SP\n"
			   <<"A=M\n"
			   <<"M=D\n"
			   <<"@SP\n"
			   <<"M=M+1\n"
			   <<"A=M\n"
			   <<"M=D\n"
			   <<"@SP\n"
			   <<"M=M+1\n";
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

bool Parser::isPushPopFuncCall() {
	Parser::commandTypes cmd = commandType();
    return cmd == Parser::C_PUSH
        || cmd == Parser::C_POP
        || cmd == Parser::C_FUNCTION
        || cmd == Parser::C_CALL;
}

bool Parser::isPushPop() {
	Parser::commandTypes cmd = commandType();
    return cmd == Parser::C_PUSH
        || cmd == Parser::C_POP;
}

const int Parser::arg2() {
	if (isPushPopFuncCall() && commandTokens.size() > 2) {
		return std::stoi(commandTokens[2]);
	}
	return -1;
}


int main() {
	const std::string& vmFile = "FunctionCalls/SimpleFunction/SimpleFunction.vm";

	Parser parser(vmFile);

	Writer writer(vmFile);

	while (parser.hasMoreCommands()) {
		//parser actions
		parser.advance();

		//writer actions
		if (parser.isPushPop()) {
			writer.writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
		}

		if (parser.commandType() == Parser::C_ARITHMETIC) {
			writer.writeArithmetic(parser.currentCommand);
		}

		if (parser.commandType() == Parser::C_LABEL) {
			writer.writeLabel(parser.arg1());
		}

		if (parser.commandType() == Parser::C_GOTO){
			writer.writeGoto(parser.arg1());
		}

		if(parser.commandType() == Parser::C_IF){
			writer.writeIf(parser.arg1());
		}

		if(parser.commandType() == Parser::C_FUNCTION){
			writer.writeFunction(parser.arg1(), parser.arg2());
		}
	}
	
	return 0;
}

/*
stage 2
implement function calling commands with call, return, and function commands
*/