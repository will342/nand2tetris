#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <cctype>
#include <unordered_map>

class JackTokenizer {

    public:
    std::vector<std::string> lines;
    std::string currentToken;
    std::vector<std::string> tokens;
    std::string symbols =  "{}()[].,;+-*/&|<>+=~";

    enum tokenTypes{
        KEYWORD,
        SYMBOL,
        IDENTIFIER,
        INT_CONST,
        STRING_CONST,
        UNKNOWN_TOKEN
    };

    std::unordered_map<std::string, tokenTypes> typeDict = {
        {"class", KEYWORD},
        {"constructor", KEYWORD},
        {"method", KEYWORD},
        {"function", KEYWORD},
        {"field", KEYWORD},
        {"static", KEYWORD},
        {"var", KEYWORD},
        {"int", KEYWORD},
        {"char", KEYWORD},
        {"boolean", KEYWORD},
        {"void", KEYWORD},
        {"true", KEYWORD},
        {"false", KEYWORD},
        {"null", KEYWORD},
        {"this", KEYWORD},
        {"let", KEYWORD},
        {"do", KEYWORD},
        {"if", KEYWORD},
        {"else", KEYWORD},
        {"while", KEYWORD},
        {"return", KEYWORD}
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

    std::unordered_map<std::string, keyWords> keywordDict = {
        {"class", CLASS},
        {"constructor", CONSTRUCTOR},
        {"method", METHOD},
        {"function", FUNCTION},
        {"field", FIELD},
        {"static", STATIC},
        {"var", VAR},
        {"int", INT},
        {"char", CHAR},
        {"boolean", BOOLEAN},
        {"void", VOID},
        {"true", TRUE},
        {"false", FALSE},
        {"null", NULL_KEYWORD},
        {"this", THIS},
        {"let", LET},
        {"do", DO},
        {"if", IF},
        {"else", ELSE},
        {"while", WHILE},
        {"return", RETURN}
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

        for (const auto& line : lines){
            std::string str = "";
            bool isString = false;

            for (const char& c: line){
                bool isSymbol = symbols.find(c) != std::string::npos;
                bool isSpace = std::isspace(c);
                bool isQuote = c == '"';

                if( isQuote){
                    isString = !isString;
                }
                
                if (!isSymbol && !isSpace && !isString){
                    str += c;
                }

                if (isSpace && !str.empty() && !isString){
                    tokens.push_back(str);
                    str = "";
                }

                if(isSymbol && !isString){
                    if (!str.empty()){
                        tokens.push_back(str);
                    }
                    tokens.push_back(std::string(1,c));
                    str = "";
                }

                if (isString){
                    str += c;
                }
            }

            if (!str.empty()){
                tokens.push_back(str);
                str = "";
            }
        }
	}
    else {
		std::cout << "Parser constructor was unable to open file" << std::endl;
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

JackTokenizer::tokenTypes JackTokenizer::tokenType(){
    //returns the type of current token (keyword, symbol, identifier, int_const, string_const)
	auto it = typeDict.find(currentToken);
	if (it != typeDict.end()) {
		return it->second;
	}
	
    if (symbols.find(currentToken) != std::string::npos){
        return SYMBOL;
    }

    bool isInt = true;
    for (const char& c : currentToken) {
        if (!isdigit(c)) {
            isInt = false;
            break;
        }
    }

    if (isInt) {
        return INT_CONST;
    }

    if (currentToken[0] == '"'){return STRING_CONST;}

    else {return IDENTIFIER;}
}

JackTokenizer::keyWords JackTokenizer::keyWord(){
    if (tokenType() == KEYWORD){
        auto it = keywordDict.find(currentToken);
        if (it != keywordDict.end()) {
            return it->second;
	    }
    }
    return UNKNOWN_KEYWORD;
}

char JackTokenizer::symbol(){
    if (tokenType() == SYMBOL){
        return currentToken[0];
    }
    return 0;
}

std::string JackTokenizer::identifier(){
    if (tokenType () == IDENTIFIER){
        return currentToken;
    }
    return "unknown identifier";
}

int JackTokenizer::intVal(){
    if (tokenType () == INT_CONST){
        return std::stoi(currentToken);
    }
    return 99;
}

std::string JackTokenizer::stringVal(){
    if (tokenType() == STRING_CONST){
        std::string str = "";

        for (const char& c: currentToken){
            if (c != '"'){
                str += c;
            }
        }
        return str;
    }
    return "unknown string const";
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

    outputFile.open("Main.xml");

	if (!outputFile.is_open()) {
        std::cout << "Parser constructor was unable to open file\n";
	}

    outputFile << "<tokens>\n";
}

int main() {
    // TO DO - test program on Square files, need to add functionality to convert all files by name
	std::ofstream outputFile;
    outputFile.open("MainT.xml");
    outputFile << "<tokens>\n";
   
    std::filesystem::path inputPath("Square/Square.jack");

    JackTokenizer jackTokenizer(inputPath);

    while (jackTokenizer.hasMoreTokens()){
        jackTokenizer.advance();

        if (jackTokenizer.tokenType() == JackTokenizer::KEYWORD){
            outputFile << "<keyword> " + jackTokenizer.currentToken << " </keyword>\n";
        }

        if (jackTokenizer.tokenType() == JackTokenizer::SYMBOL) {
            char c = jackTokenizer.symbol();
            std::string output;

            if (c == '<') {output = "&lt;";}
            else if (c == '>') {output = "&gt;";}
            else if (c == '"') {output = "&quot;";}
            else if (c == '&') {output = "&amp;";}
            else {output = std::string(1, c);}

            outputFile << "<symbol> " << output << " </symbol>\n";
        }

        if (jackTokenizer.tokenType() == JackTokenizer::IDENTIFIER){
            outputFile << "<identifier> " + jackTokenizer.currentToken << " </identifier>\n";
        }
        
        if (jackTokenizer.tokenType() == JackTokenizer::INT_CONST){
            outputFile << "<integerConstant> " + jackTokenizer.currentToken << " </integerConstant>\n";
        }
        
        if (jackTokenizer.tokenType() == JackTokenizer::STRING_CONST){
            outputFile << "<stringConstant> " + jackTokenizer.stringVal() << " </stringConstant>\n";
        }

        std::cout << std::endl;
    }

    outputFile << "</tokens>";

    CompilationEngine compilationEngine;

    //jackTokenizer.printLines();
  
    //jackTokenizer.printTokens();
  
    return 0;
}