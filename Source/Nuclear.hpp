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
  private:
    int argc;
    char** argv;
    std::string output, input;
};

class Token {
  public:
    Token(std::string value, std::string type, int line, int column) {
      this->value = value;
      this->type = type;
      this->line = line;
      this->column = column;
    }
    std::string getValue() { return value; }
    std::string getType() { return type; }
    int getLine() { return line; }
    int getColumn() { return column; }
    std::string value, type;
    int line, column;
};

class Nuclear {
  public:
    Nuclear(Arguments* args);
  private:
    Arguments* args;
    std::vector<Token> tokens;
};

#endif // Nuclear_hpp