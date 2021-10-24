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

class Nuclear {
  public:
    Nuclear(Arguments* args);
  private:
    Arguments* args;
    std::vector<std::variant<int, float, double, std::string>> tokens;
};

#endif // Nuclear_hpp