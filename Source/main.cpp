#include <iostream>
#include <fstream>
#include <Error.hpp>
#include <Position.hpp>
#include <Token.hpp>
#include <Variable.hpp>
#include <vector>
#include <algorithm>

std::vector<std::string> reservedWords = {"do", "while", "if", "for", "else", "switch", "case", "continue", "break", "return", "let", "const", "var", "class", "struct", "enum"};

int main() {
    std::string path = "test.nuke";
    std::ifstream file(path);
    std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    data += "\n\0";
    std::vector<Token> tokens;
    std::vector<Error> errors;

    // Context Data
    char previousChar = 0;
    // String
    bool inString = false;
    char stringInitializer = 0;
    std::string currentString = "";
    // Name
    bool inName = false;
    std::string currentName = "";
    // Number
    bool inNumber = false;
    std::string currentNumber = "";
    char numberType = 0;
    // Operators
    bool inOperator = false;
    std::string currentOperator = "";

    // Tokenize
    int line = 1;
    int column = 1;
    for (char c: data) {
        if (inString) {
            if (c == stringInitializer && previousChar != '\\') {
                inString = false;
                stringInitializer = 0;
                Token token;
                token.SetType("String");
                token.SetValue(currentString);
                token.SetPosition(Position(line, column-currentString.size()), Position(line, column-1));
                tokens.push_back(token);
                currentString = "";
                continue;
            } else if (c == '\n') {
                Error error;
                error.SetPath(path);
                error.SetPosition(Position(line, column-currentString.size()-1));
                error.SetMessage(std::string("Unterminated String: ") + currentString);
                error.SetType(2);
                errors.push_back(error);
                inString = false;
                stringInitializer = 0;
                currentString = "";
                continue;
            }
            if (previousChar == '\\') {
                switch (c) {
                    case 'n':
                        currentString+='\n';
                        break;
                    case 't':
                        currentString+='\t';
                        break;
                    case 'r':
                        currentString+='\r';
                        break;
                    case '0':
                        currentString+='\0';
                        break;
                    case '\\':
                        currentString+='\\';
                        break;
                    case '"':
                    case '\'':
                    case '`':
                        currentString+=c;
                        break;
                    default:
                        Error error;
                        error.SetPath(path);
                        error.SetPosition(Position(line, column-1), Position(line, column+1));
                        error.SetMessage(std::string("Unknown string literal: '\\") + c + "'");
                        error.SetType(2);
                        errors.push_back(error);
                        break;
                }
            }
            column++;
            previousChar = c;
            if (c == '\\') continue;
            currentString+=c;
            continue;
        } else if (inName) {
            switch (c) {
                case 'A' ... 'Z':
                case 'a' ... 'z':
                case '0' ... '9':
                case '_':
                    column++;
                    previousChar = c;
                    currentName+=c;
                    continue;
                    break;
                default:
                    inName = false;
                    if (currentName == "true" || currentName == "false") {
                        Token token;
                        token.SetType("Boolean");
                        token.SetValue(currentName);
                        token.SetPosition(Position(line, column-currentName.size()), Position(line, column-1));
                        tokens.push_back(token);
                        currentName = "";
                        break;
                    } else if (currentName == "null") {
                        Token token;
                        token.SetType("Address");
                        token.SetValue("0x0");
                        token.SetPosition(Position(line, column-currentName.size()), Position(line, column-1));
                        tokens.push_back(token);
                        currentName = "";
                        break;
                    } else if (std::find(reservedWords.begin(), reservedWords.end(), currentName) != reservedWords.end()) {
                        Token token;
                        token.SetType("Reserved");
                        token.SetValue(currentName);
                        token.SetPosition(Position(line, column-currentName.size()), Position(line, column-1));
                        tokens.push_back(token);
                        currentName = "";
                        break;
                    }
                    Token token;
                    token.SetType("Name");
                    token.SetValue(currentName);
                    token.SetPosition(Position(line, column-currentName.size()), Position(line, column-1));
                    tokens.push_back(token);
                    currentName = "";
                    break;
            }
        } else if (inNumber) {
            switch (numberType) {
                case 'h': // Hexadecimal
                    switch (c) {
                        case 'A' ... 'F':
                        case 'a' ... 'f':
                        case '0' ... '9':
                            column++;
                            previousChar = c;
                            currentNumber+=c;
                            continue;
                            break;
                        default:
                            inNumber = false;
                            Token token;
                            token.SetType("Hex");
                            token.SetValue(currentNumber);
                            token.SetPosition(Position(line, column-currentNumber.size()), Position(line, column-1));
                            tokens.push_back(token);
                            currentNumber = "";
                            numberType = 0;
                            break;
                    }
                    break;
                case 'b': // Binary
                    switch (c) {
                        case '0':
                        case '1':
                            column++;
                            previousChar = c;
                            currentNumber+=c;
                            continue;
                            break;
                        default:
                            inNumber = false;
                            Token token;
                            token.SetType("Binary");
                            token.SetValue(currentNumber);
                            token.SetPosition(Position(line, column-currentNumber.size()), Position(line, column-1));
                            tokens.push_back(token);
                            currentNumber = "";
                            numberType = 0;
                            break;
                    }
                    break;
                case 'f': // Float
                    switch (c) {
                        case '0' ... '9':
                        case '.':
                            column++;
                            previousChar = c;
                            currentNumber+=c;
                            continue;
                            break;
                        case '\0':
                        case ' ':
                        case '\t':
                        case '\n':
                        case ';': {
                            inNumber = false;
                            Token token;
                            token.SetType("Float");
                            token.SetValue(currentNumber);
                            token.SetPosition(Position(line, column-currentNumber.size()), Position(line, column-1));
                            tokens.push_back(token);
                            currentNumber = "";
                            numberType = 0;
                            continue;
                            break;
                        }
                        default:
                            inNumber = false;
                            Token token;
                            token.SetType("Float");
                            token.SetValue(currentNumber);
                            token.SetPosition(Position(line, column-currentNumber.size()), Position(line, column-1));
                            tokens.push_back(token);
                            currentNumber = "";
                            numberType = 0;
                            break;
                    }
                default:
                    if (currentNumber == "0") {
                        switch (c) {
                            case 'x':
                                numberType = 'h';
                                column++;
                                previousChar = c;
                                currentNumber = "";
                                continue;
                                break;
                        }
                    }
                    switch (c) {
                        case '0' ... '9':
                            column++;
                            previousChar = c;
                            currentNumber+=c;
                            continue;
                            break;
                        case 'b':
                            for (char num: currentNumber) {
                                if (num != '0' && num != '1') {
                                    Error error;
                                    error.SetPath(path);
                                    error.SetPosition(Position(line, column-currentNumber.size()), Position(line, column+1));
                                    error.SetMessage(std::string("Invalid binary number: ") + currentNumber);
                                    error.SetType(2);
                                    errors.push_back(error);
                                    inNumber = false;
                                    numberType = 0;
                                    currentNumber = "";
                                    continue;
                                    break;
                                }
                            }
                            numberType = 'b';
                            column++;
                            previousChar = c;
                            continue;
                            break;
                        case '.':
                            currentNumber+=c;
                        case 'f':
                            numberType = 'f';
                            column++;
                            previousChar = c;
                            continue;
                            break;
                        default:
                            inNumber = false;
                            Token token;
                            token.SetType("Number");
                            token.SetValue(currentNumber);
                            token.SetPosition(Position(line, column-currentNumber.size()), Position(line, column-1));
                            tokens.push_back(token);
                            currentNumber = "";
                            numberType = 0;
                            break;
                    }
                    break;
            }
        } else if (inOperator) {
            switch (c) {
                case '=':
                case '+':
                case '-':
                case '*':
                case '/':
                case '%':
                case '&':
                case '|':
                case '^':
                case '~':
                case '<':
                case '>':
                case '!': {
                    currentOperator+=c;
                    Token token;
                    token.SetType("Operator");
                    token.SetValue(currentOperator);
                    token.SetPosition(Position(line, column-currentOperator.size()), Position(line, column));
                    tokens.push_back(token);
                    inOperator = false;
                    currentOperator = "";
                    continue;
                    break;
                }
                default:
                    inOperator = false;
                    Token token;
                    token.SetType("Operator");
                    token.SetValue(currentOperator);
                    token.SetPosition(Position(line, column-currentOperator.size()), Position(line, column));
                    tokens.push_back(token);
                    currentOperator = "";
                    break;
            }
        }
        switch (c) {
            case '\n':
                line++;
                column = 0;
            case ';': {
                Token token;
                token.SetType("EOS");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            case ' ':
            case '\t':
            case '\0':
                break;
            case 'A' ... 'Z':
            case 'a' ... 'z':
                inName = true;
                currentName+=c;
                break;
            case '0' ... '9':
                inNumber = true;
                currentNumber+=c;
                break;
            case '(':
            case ')': {
                Token token;
                token.SetType("Parenthesis");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            case '{':
            case '}': {
                Token token;
                token.SetType("Curly Bracket");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            case '[':
            case ']': {
                Token token;
                token.SetType("Bracket");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            case '"':
            case '\'':
            case '`':
                inString = true;
                stringInitializer = c;
                break;
            case '.': {
                Token token;
                token.SetType("Dot");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            case ',': {
                Token token;
                token.SetType("Seperator");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            case '=':
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
            case '&':
            case '|':
            case '^':
            case '~':
            case '!':
            case '<':
            case '>':
                inOperator = true;
                currentOperator+=c;
                break;
            case '?': {
                Token token;
                token.SetType("Question Mark");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            case ':': {
                Token token;
                token.SetType("Colon");
                token.SetValue(std::string(1, c));
                token.SetPosition(Position(line, column), Position(line, column));
                tokens.push_back(token);
                break;
            }
            default:
                Error error;
                error.SetPath(path);
                error.SetPosition(Position(line, column), Position(line, column+1));
                error.SetMessage(std::string("Invalid token '")+c+"'!");
                error.SetType(2);
                errors.push_back(error);
                break;
        }
        column++;
        previousChar = c;
    }

    std::vector<Variable> variables;

    // Horrible logic below, will modify later

    int i = 0;
    for (Token token: tokens) {
        // std::cout << token.GetType() << ": '" << token.GetValue() << "'" << std::endl;
        if (token.GetType() == "Reserved") {
            if (token.GetValue() == reservedWords[10]) { // let
                if (tokens[i+1].GetType() != "Name") {
                    Error error;
                    error.SetPath(path);
                    error.SetPosition(tokens[i+1].GetPosition()[0], tokens[i+1].GetPosition()[1]);
                    error.SetMessage("Expected name after 'let'!");
                    error.SetType(2);
                    errors.push_back(error);
                    continue;
                }
                Variable variable = Variable(tokens[i+1].GetValue());
                variables.push_back(variable);
            }
        }
        i++;
    }

    i=0;
    int x = 0;
    for (Token token: tokens) {
        if (token.GetValue() == "(") {

            bool done = false;
            int parenthesis = 1;
            std::vector<Token> tokensInParenthesis;
            x=i;
            while (!done) {
                x++;
                if (tokens[x].GetValue() == "(") parenthesis++;
                else if (tokens[x].GetValue() == ")") {
                    if (parenthesis == 1) {
                        done = true;
                        break;
                    } else parenthesis--;
                }
                if (tokens[x].GetType() == "EOS" && tokens[x].GetValue() != ";") {
                    Error error;
                    error.SetPath(path);
                    error.SetPosition(tokens[x-1].GetPosition()[0]);
                    error.SetMessage("Expected ')'!");
                    error.SetType(2);
                    errors.push_back(error);
                    break;
                }
                tokensInParenthesis.push_back(tokens[x]);
            }
            tokens.erase(tokens.begin()+i, tokens.begin()+x);
            for (Token token: tokensInParenthesis) std::cout << token.GetValue();
            std::cout << std::endl;
        }
        i++;
    }
    
    std::cout << std::endl;

    for (Variable variable: variables) {
        std::cout << variable.GetName() << " = ";
        if (variable.value.GetType() != "") std::cout << variable.value.GetType() << ": " << variable.value.GetValue() << std::endl;
        else std::cout << "null" << std::endl;
    }

    for (Error error: errors) error.Print();
    return 0;
}