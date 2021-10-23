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
          else output = argv[i];
        }
      }
    }
  }
}