#ifndef Nuclear
#define Nuclear

#define Version 0.1

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

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

#endif