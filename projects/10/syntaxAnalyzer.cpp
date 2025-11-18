#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <sstream>

class JackTokenizer {

    public:
    std::vector<std::string> lines;
    std::string currentToken;
    std::vector<std::string> tokens;

    enum tokenTypes{
        KEYWORD,
        SYMBOL,
        IDENTIFIER,
        INT_CONST,
        STRING_CONST,
        UNKNOWN_TOKEN
    };

    enum keyWords{
        CLASS,
        METHOD,
        FUNCTION,
        CONSTRUCTOR,
        INT,
        BOOLEAN,
        CHAR,
        VOID,
        VAR,
        STATIC,
        FIELD,
        LET,
        DO,
        IF,
        ELSE,
        WHILE,
        RETURN,
        TRUE,
        FALSE,
        NULL_KEYWORD,
        THIS,
        UNKNOWN_KEYWORD
    };

    JackTokenizer(const std::string& inputFilename);
    void printLines();
    void printTokens();
    bool hasMoreTokens();
    void advance();
    tokenTypes tokenType();
    keyWords keyWord();
    char symbol();
    std::string identifier();
    int intVal();
    std::string stringVal();
};

JackTokenizer::JackTokenizer(const std::string& inputFilename) {
	std::ifstream inputFile(inputFilename);
	if (inputFile.is_open()) {
		std::string line;
        bool isCommentBlock = false;
        
		while (getline(inputFile, line)) {
            size_t commentPos;
            
			// Find and remove "// lines"
        	commentPos = line.find("//");
        	if (commentPos != std::string::npos) {
                line.erase(commentPos);
        	}

            //find and remove /* and /** comment blocks
            commentPos = line.find("/**");
            if (commentPos != std::string::npos) {
                isCommentBlock = true;
        	}

            commentPos = line.find("/*");
            if (commentPos != std::string::npos) {
                isCommentBlock = true;
        	}

            commentPos = line.find("*/");
            if (commentPos != std::string::npos) {
                isCommentBlock = false;
                line.erase();
        	}
            
			// Trim whitespaces
       		auto notSpace = [](int ch){ return !std::isspace(ch); };
			line.erase(line.begin(), std::find_if(line.begin(), line.end(), notSpace));
			line.erase(std::find_if(line.rbegin(), line.rend(), notSpace).base(), line.end());

			// Add line to lines vector
			if(!line.empty() && !isCommentBlock){
				lines.push_back(line);
			}
		}
	}
	else {
		std::cout << "Parser constructor was unable to open file" << std::endl;
	}

    for (const auto& line : lines){
        // to do: seperate out symbols into new elements
        std::stringstream ss(line);
        std::string token;

        while (ss >> token){
            tokens.push_back(token);
        }
    }
}

void JackTokenizer::printLines() {
	for (const auto& line : lines) {
		std::cout << line << std::endl;
	}
}

void JackTokenizer::printTokens() {
	for (const auto& token : tokens) {
		std::cout << token << std::endl;
	}
}


bool JackTokenizer::hasMoreTokens(){
    return !tokens.empty();
}

void JackTokenizer::advance(){
    if (hasMoreTokens()){
        currentToken = tokens.front();
        tokens.erase(tokens.begin());
    }
}

class CompilationEngine{
    public: 
    CompilationEngine();

    void CompileClass();
    void CompileClassVarDec();
    void CompileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compilteReturn();
    void compileIf();
    void compileTerm();
    void compileExpressionList();
};

CompilationEngine::CompilationEngine(){
        
    std::ofstream outputFile;

    outputFile.open("testOutput.xml");

	if (outputFile.is_open()) {
        std::cout << "output file open \n";
	}

	else {
		std::cout << "Parser constructor was unable to open file" << std::endl;
	}
}

int main() {
	std::ofstream outputFile;
   
    std::filesystem::path inputPath("ArrayTest/Main.jack");

    JackTokenizer jackTokenizer(inputPath);
    CompilationEngine compilationEngine;

    jackTokenizer.printLines();
  
    std::cout <<"Tokens\n";

    jackTokenizer.printTokens();
  
    return 0;
}