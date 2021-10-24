#include "Nuclear.hpp"

std::string toLower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

Arguments::Arguments(int argc, char** argv) {
  this->argc = argc;
  this->argv = argv;
  if (argc == 1) {
    std::cout << "Nuclear v" << Version << std::endl;
  } else if (argc >= 2) {
    if (std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v") std::cout << "Nuclear v" << Version << std::endl;
    else {
      for (unsigned int i = 1; i < argc; i++) {
        if (std::ifstream(argv[i]).good()) {
          std::string extension = std::string(argv[i]).substr(std::string(argv[i]).find_last_of(".") + 1);
          if (toLower(extension) == "nuke" || toLower(extension) == "nuclear") input = argv[i];
        } else {
          std::string extension = std::string(argv[i]).substr(std::string(argv[i]).find_last_of(".") + 1);
          if (toLower(extension) == "nuke" || toLower(extension) == "nuclear") {
            std::cout << "File '" << argv[i] << "' does not exist!" << std::endl;
            std::exit(-1);
          }
          else {
            if (input != "") output = argv[i];
            else {
              std::cout << "Please specify an input file!" << std::endl;
              std::exit(-1);
            }
          }
        }
      }
    }
  }
}

Nuclear::Nuclear(Arguments* args) { 
  this->args = args;
  std::string data = "";
  std::ifstream file(args->getInput());
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) data += line + "\n";
    file.close();
  } else {
    std::cout << "Could not open file '" << args->getInput() << "'!" << std::endl;
    std::exit(-1);
  }
}