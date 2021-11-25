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
  std::cout << data << std::endl;

  std::string toks = "";
  char PreviousChar = '\0';
  bool IsInQuotes = false;
  std::string quote = "";
  char QuoteInitiator = '\0';
  int IsEscaped = 0; 
  for (char& c : data) {
    if (IsInQuotes) {
      if (c == '\\') {
        IsEscaped++;
        if (IsEscaped == 2) {
          quote += '\\';
          IsEscaped = 0;
        }
      } else if (c == 'n' || c == 't' || c == 'b' || c == 'r') {
        if (IsEscaped == 1) {
          switch (c) {
            case 'n': quote += '\n'; break;
            case 't': quote += '\t'; break;
            case 'b': quote += '\b'; break;
            case 'r': quote += '\r'; break;
          }
          IsEscaped = 0;
        } else {
          quote += c;
        }
      } else if (c == '\"' || c == '\'') {
        if (IsEscaped == 1) {
          quote+=c;
          IsEscaped = 0;
        } else if ((c == '\"' && QuoteInitiator == '\"') || (c == '\'' && QuoteInitiator == '\'')) {
          quote+='"';
          IsInQuotes = false;
          tokens.push_back(quote);
        }
      } else quote+=c;
    } else {
      toks+=c;
      if (toks == "print") {
        tokens.push_back(toks);
        toks = "";
      } else if (toks == "(" || toks == ")" || toks == "{" || toks == "}") {
        tokens.push_back(toks);
        toks = "";
      } else if (toks == " ") {
        toks = "";
      } else if (toks == "\n") {
        tokens.push_back("\n");
      } else if (toks == ";") {
        tokens.push_back(";");
      } else if (toks == "\"" || toks == "'") {
        IsInQuotes = true;
        QuoteInitiator = toks[0];
        quote = "\"";
        toks = "";
      }
    }
    PreviousChar = c;
    std::cout << toks << std::endl;
  }

  int i=0;
  for (auto& s : tokens) {
    if (i == 0 && i == tokens.size()-1) std::cout << '[' << std::get<std::string>(s) << ']' << std::endl;
    else if (i == 0) std::cout << '[' << std::get<std::string>(s) << ", ";
    else if (i == tokens.size()-1) std::cout << std::get<std::string>(s) << ']' << std::endl;
    else std::cout << std::get<std::string>(s) << ", ";
    i++;
  }

}