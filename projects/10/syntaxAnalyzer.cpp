#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <cctype>
#include <unordered_map>
namespace fs = std::filesystem;

class JackTokenizer {

    public:
    std::vector<std::string> lines;
    std::string currentToken;
    std::vector<std::string> tokens;
    std::string symbols =  "{}()[].,;+-*/&|<>+=~";
    size_t currentPos = 0;

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

    std::unordered_map<std::string, keyWords> keywordMap = {
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
    void retreat();
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
    return currentPos < tokens.size();
}

void JackTokenizer::advance(){
    if (hasMoreTokens()){
        currentToken = tokens[currentPos];
        currentPos++;
    }
}

void JackTokenizer::retreat(){
    if (currentPos !=0){currentPos -= 1;} 
}

JackTokenizer::tokenTypes JackTokenizer::tokenType(){
    //returns the type of current token (keyword, symbol, identifier, int_const, string_const)
	auto it = keywordMap.find(currentToken);
	if (it != keywordMap.end()) {
		return KEYWORD;
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
    //returns the keyword type
    if (tokenType() == KEYWORD){
        auto it = keywordMap.find(currentToken);
        if (it != keywordMap.end()) {
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
    private:
    std::ofstream outputFile;
    JackTokenizer& tokenizer;
    int programLevel = 1;
    std::string indent = "";
    
    public: 
    CompilationEngine(JackTokenizer&, fs::path&);

    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
    void writeToken();
    void setIndent();
};

CompilationEngine::CompilationEngine(JackTokenizer& tok, fs::path& outputPath)
    : tokenizer(tok), outputFile(outputPath) {
    outputFile << "<class>\n";    
    }

void CompilationEngine::compileClass(){
    while (tokenizer.hasMoreTokens()){    
        tokenizer.advance();

        if (tokenizer.keyWord() == JackTokenizer::STATIC || tokenizer.keyWord() == JackTokenizer::FIELD){
            compileClassVarDec();    
        }

        else if (tokenizer.keyWord() == JackTokenizer::CONSTRUCTOR
         || tokenizer.keyWord() == JackTokenizer::FUNCTION
         || tokenizer.keyWord() == JackTokenizer::METHOD
         || tokenizer.keyWord() == JackTokenizer::VOID){
            compileSubroutine();
        }

        else{
            writeToken();
        }
    }

    if(!tokenizer.hasMoreTokens()){
        outputFile << "</class>";
    } 
}

void CompilationEngine::compileClassVarDec(){
    bool inClassVarDec = true;
    outputFile << indent << "<classVarDec>\n";
    programLevel ++;
    setIndent();
    writeToken();

    while (tokenizer.hasMoreTokens() && inClassVarDec){
    tokenizer.advance();
    writeToken();
        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == ';') ){
            inClassVarDec = false;
        }
    }

    programLevel -= 1;
    setIndent();
    outputFile << indent << "</classVarDec>\n";

}

void CompilationEngine::compileSubroutine(){
    bool inSubroutine = true;
    outputFile << indent << "<subroutineDec>\n";
    programLevel ++;
    writeToken();

    while (tokenizer.hasMoreTokens() && inSubroutine){
        bool write = true;
        tokenizer.advance();    
        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == '{') ){
            outputFile << indent << "<subroutineBody>\n";
            programLevel++;
        }

        if(tokenizer.keyWord() == JackTokenizer::VAR){
            compileVarDec();
            write = false;
        }

        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == '(') ){
            writeToken();
            compileParameterList();
            write = false;
        }

        if (tokenizer.keyWord() == JackTokenizer::LET
            ||tokenizer.keyWord() == JackTokenizer::IF
            ||tokenizer.keyWord() == JackTokenizer::WHILE
            ||tokenizer.keyWord() == JackTokenizer::DO
            ||tokenizer.keyWord() == JackTokenizer::RETURN){
            compileStatements();
            write = false;    
            }

        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == '}') ){
            writeToken();
            inSubroutine = false;
            programLevel -= 1;
            setIndent();
            outputFile << indent << "</subroutineBody>\n";
            write = false;
        }

        if(write) {writeToken();}
    }

    programLevel -= 1;
    setIndent();
    outputFile << indent << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList(){
    bool inParameterList = true;
    outputFile << indent << "<parameterList>\n";
    programLevel ++;

    while (tokenizer.hasMoreTokens() && inParameterList){
        tokenizer.advance();
        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == ')') ){
            inParameterList = false;
        }

        if (inParameterList){writeToken();}    
    }

    programLevel -= 1;
    setIndent();
    outputFile << indent << "</parameterList>\n";
    writeToken();
}

void CompilationEngine::compileVarDec(){
    bool inVarDec = true;
    outputFile << indent << "<varDec>\n";
    programLevel ++;
    writeToken();

    while (tokenizer.hasMoreTokens() && inVarDec){
        tokenizer.advance();
        writeToken();
        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == ';') ){
            inVarDec = false;
        }
    }

    programLevel -= 1;
    setIndent();
    outputFile << indent << "</varDec>\n";
}

void CompilationEngine::compileStatements(){
    bool inStatements = true;
    outputFile << indent << "<statements>\n";
    programLevel++;
    setIndent();

    while (tokenizer.hasMoreTokens() && inStatements){
        if (tokenizer.keyWord() == JackTokenizer::LET){
            compileLet();
            tokenizer.advance();
        }
             
        else if (tokenizer.keyWord() == JackTokenizer::IF){
            compileIf();
            tokenizer.advance();
        }

        else if (tokenizer.keyWord() == JackTokenizer::WHILE){
            compileWhile();
            tokenizer.advance();
        }

        else if (tokenizer.keyWord() == JackTokenizer::DO){
            compileDo();
            tokenizer.advance();
        }

        else if (tokenizer.keyWord() == JackTokenizer::RETURN){
            compileReturn();
            tokenizer.advance();
        }

        else {inStatements = false;}
    }
    
    programLevel -= 1;
    setIndent();
    outputFile << indent << "</statements>\n";
}

void CompilationEngine::compileLet(){
    bool inLetStatement = true;
    outputFile << indent << "<letStatement>\n";
    programLevel ++;
    setIndent();

    while (tokenizer.hasMoreTokens() && inLetStatement){
        writeToken();
        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == ';') ){
            inLetStatement = false;
        }
        if (tokenizer.symbol() == '='){
            tokenizer.advance();
            compileExpression();
        }
        if (inLetStatement){tokenizer.advance();}
    }

    programLevel -= 1;
    setIndent();
    outputFile << indent << "</letStatement>\n";
}

void CompilationEngine::compileIf(){
    bool inIfStatement = true;
    outputFile << indent << "<ifStatement>\n";
    programLevel ++;
    setIndent();

    while (tokenizer.hasMoreTokens() && inIfStatement){
        writeToken();
         
        if (tokenizer.symbol() == '('){
            tokenizer.advance();
            compileExpression();
        }
        if(tokenizer.symbol() == '{'){ 
            tokenizer.advance();
            compileStatements();
        }

        if (tokenizer.symbol() == '}'){
            writeToken();
            inIfStatement = false;
        }

        if (inIfStatement){tokenizer.advance();}
    }

    tokenizer.advance(); 
    if (tokenizer.keyWord() == JackTokenizer::ELSE){
                writeToken();
                tokenizer.advance();
                writeToken();
                compileStatements(); 
                tokenizer.advance();
                writeToken();
                inIfStatement = false;
    }
    else{tokenizer.retreat();}

    programLevel -= 1;
    setIndent();
    outputFile << indent << "</ifStatement>\n";
}

void CompilationEngine::compileWhile(){
    bool inWhileStatement = true;
    outputFile << indent << "<whileStatement>\n";
    programLevel ++;
    setIndent();

    while (tokenizer.hasMoreTokens() && inWhileStatement){
        writeToken();
        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == ';') ){
            inWhileStatement = false;
        }

        if(tokenizer.symbol() == '('){
            tokenizer.advance();
            compileExpression();
        }

        if(tokenizer.symbol() == '{'){ 
            tokenizer.advance();
            compileStatements();
        }

        if (tokenizer.symbol() == '}'){
            writeToken();
            inWhileStatement = false;
        }

        if (inWhileStatement){tokenizer.advance();}
    }
    programLevel -= 1;
    setIndent();
    outputFile << indent << "</whileStatement>\n";
}

void CompilationEngine::compileDo(){
    bool inDoStatement = true;
    outputFile << indent << "<doStatement>\n";
    programLevel ++;
    setIndent();

    while (tokenizer.hasMoreTokens() && inDoStatement){
        writeToken();
        if ((tokenizer.tokenType() == JackTokenizer::SYMBOL) && (tokenizer.symbol() == ';') ){
            inDoStatement = false;
        }

        if(tokenizer.symbol() == '('){
            compileExpressionList();
        }
        if (inDoStatement){tokenizer.advance();}
    }
    programLevel -= 1;
    setIndent();
    outputFile << indent << "</doStatement>\n";
}

void CompilationEngine::compileReturn(){
    bool inReturnStatement = true;
    outputFile << indent << "<returnStatement>\n";
    programLevel ++;
    setIndent();
    writeToken();
    tokenizer.advance();

    if (tokenizer.symbol() != ';'){
        compileExpression();
        tokenizer.advance();
    }
    if (tokenizer.symbol() == ';'){
        writeToken();
    }
    programLevel -= 1;
    setIndent();
    outputFile << indent << "</returnStatement>\n";
}

void CompilationEngine::compileExpressionList(){
    bool inExpressionList = true;
    tokenizer.advance();
    outputFile << indent << "<expressionList>\n";

    while (tokenizer.symbol() != ')') {
            if (tokenizer.tokenType() == JackTokenizer::SYMBOL){
                writeToken();
            }
            else {compileExpression();}
            tokenizer.advance();
    }

    outputFile << indent<< "</expressionList>\n";
    if (tokenizer.symbol() == ')'){
        writeToken();
    }
}

void CompilationEngine::compileExpression(){
    outputFile << indent << "<expression>\n";
    compileTerm();
    outputFile << indent << "</expression>\n";
}

void CompilationEngine::compileTerm(){
    programLevel++;
    setIndent();
    outputFile << indent << "<term>\n";
    programLevel++;
    setIndent();
    writeToken();
    programLevel -= 1;
    setIndent();
    outputFile << indent << "</term>\n";
    programLevel -= 1;
    setIndent();
}

void CompilationEngine::writeToken(){
    setIndent();

    switch (tokenizer.tokenType()){
    case JackTokenizer::KEYWORD:
        outputFile << indent << "<keyword> " + tokenizer.currentToken << " </keyword>\n";
        break;

    case JackTokenizer::SYMBOL: {
        char c = tokenizer.symbol();
        std::string output;

        if (c == '<') {output = "&lt;";}
        else if (c == '>') {output = "&gt;";}
        else if (c == '"') {output = "&quot;";}
        else if (c == '&') {output = "&amp;";}
        else {output = std::string(1, c);}

        outputFile << indent << "<symbol> " << output << " </symbol>\n";
        
        break;
    }

    case JackTokenizer::IDENTIFIER:
        outputFile << indent << "<identifier> " << tokenizer.identifier() << " </identifier>\n";
        break;

    case JackTokenizer::INT_CONST:
        outputFile << indent << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>\n";
        break;

    case JackTokenizer::STRING_CONST:
        outputFile << indent << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>\n";
        break;

    case JackTokenizer::UNKNOWN_TOKEN:
        outputFile << indent <<"UNKNOWN TOKEN\n";
        break;
    }
}

void CompilationEngine::setIndent(){
    indent = "";
    for (int i = 0 ; i < programLevel; i++){
        indent += "  ";
    }
}


int main() {

    fs::path programFolder = "ExpressionLessSquare";
    fs::path outputPath;
    std::vector<fs::path> jackFilePaths;

    // Generate xxxT.xml files for tokenizer testing
    for (const auto & entry : fs::directory_iterator(programFolder))
    if (entry.path().extension() == ".jack"){
        jackFilePaths.push_back(entry.path());
    } 

    for (const auto & path : jackFilePaths){
        fs::path inputPath = path;
        outputPath = path.stem().string() + "T.xml";
        std::ofstream outputFile(outputPath);
        outputFile << "<tokens>\n";

        JackTokenizer jackTokenizer(inputPath);

        while (jackTokenizer.hasMoreTokens()){
            jackTokenizer.advance();

            switch (jackTokenizer.tokenType()){
                case JackTokenizer::KEYWORD:
                    outputFile << "<keyword> " + jackTokenizer.currentToken << " </keyword>\n";
                    break;

                case JackTokenizer::SYMBOL: {
                    char c = jackTokenizer.symbol();
                    std::string output;

                    if (c == '<') {output = "&lt;";}
                    else if (c == '>') {output = "&gt;";}
                    else if (c == '"') {output = "&quot;";}
                    else if (c == '&') {output = "&amp;";}
                    else {output = std::string(1, c);}

                    outputFile << "<symbol> " << output << " </symbol>\n";
                    
                    break;
                }

                case JackTokenizer::IDENTIFIER:
                    outputFile << "<identifier> " << jackTokenizer.identifier() << " </identifier>\n";
                    break;

                case JackTokenizer::INT_CONST:
                    outputFile << "<integerConstant> " << jackTokenizer.intVal() << " </integerConstant>\n";
                    break;

                case JackTokenizer::STRING_CONST:
                    outputFile << "<stringConstant> " << jackTokenizer.stringVal() << " </stringConstant>\n";
                    break;

                case JackTokenizer::UNKNOWN_TOKEN:
                    outputFile << "UNKNOWN_TOKEN\n";
                    break;
            }

            std::cout << std::endl;
        }

        outputFile << "</tokens>";
    }

    //Generate final .xml files using compilation engine
    for (const auto & path : jackFilePaths){
        fs::path inputPath = path;
        outputPath = path.stem().string() + ".xml";
        std::ofstream outputFile(outputPath);

        JackTokenizer jackTokenizer(inputPath);
        CompilationEngine compilationEngine (jackTokenizer, outputPath);
        
        compilationEngine.compileClass();


        }

        //jackTokenizer.printLines();
    
        //jackTokenizer.printTokens();
    
    return 0;
}