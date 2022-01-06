#ifndef Nuclear_hpp
#define Nuclear_hpp

#define Version 0.1

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <variant>

class Arguments {
  public:
    Arguments(int argc, char** argv);
    std::string getOutput() { return output; }
    std::string getInput() { return input; }
    bool getIsLibEnabled() { return isLibEnabled; }
    bool getIsShowingTime() { return showTime; }
  private:
    int argc;
    char** argv;
    std::string output, input;
    bool isLibEnabled, showTime;
};

class Token {
  public:
    Token(std::string value, std::string type, int line, int column, std::string path) {
      this->value = value;
      this->type = type;
      this->line = line;
      this->column = column;
      this->path = path;
    }
    std::string getValue() { return value; }
    std::string getType() { return type; }
    int getLine() { return line; }
    int getColumn() { return column; }
    std::string getPath() { return path; }
    std::string value, type, path;
    int line, column;
};

class Nuclear {
  public:
    Nuclear(Arguments* args);
  private:
    Arguments* args;
    std::vector<Token> tokens;
    void Calculate(int i, int& MathOperators, std::string type), Compiler(), Lexer(std::string path);
    std::vector<std::string> imports;
    std::vector<std::vector<std::string>> lines;
    std::vector<std::string> paths;
};

#endif // Nuclear_hpp
