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
  bool IsInInt = false;
  std::string number = "";
  bool IsInName = false;
  std::string name = "";
  bool IsInMLComment = false;
  bool MLCommentEnd = false;
  bool IsInComment = false;
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
      } else if (c == '\"' || c == '\'' || c == '`') {
        if (IsEscaped == 1) {
          quote+=c;
          IsEscaped = 0;
        } else if ((c == '\"' && QuoteInitiator == '\"') || (c == '\'' && QuoteInitiator == '\'') || (c == '`' && QuoteInitiator == '`')) {
          quote+='"';
          IsInQuotes = false;
          tokens.push_back(quote);
        } else {
          quote+=c;
        }
      } else quote+=c;
    } else {
      bool IsContinued = true;
      if (IsInInt) {
        if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9' && c != '.') {
          tokens.push_back(number);
          number = "";
          IsInInt = false;
        } else {
          number += c;
          IsContinued = false;
        }
      } else if (IsInName) {
        if (c != 'a' && c != 'b' && c != 'c' && c != 'd' && c != 'e' && c != 'f' && c != 'g' && c != 'h' && c != 'i' && c != 'j'
        && c != 'k' && c != 'l' && c != 'm' && c != 'n' && c != 'o' && c != 'p' && c != 'q' && c != 'r' && c != 's' && c != 't'
        && c != 'u' && c != 'v' && c != 'w' && c != 'x' && c != 'y' && c != 'z' && c != 'A' && c != 'B' && c != 'C' && c != 'D'
        && c != 'E' && c != 'F' && c != 'G' && c != 'H' && c != 'I' && c != 'J' && c != 'K' && c != 'L' && c != 'M' && c != 'N'
        && c != 'O' && c != 'P' && c != 'Q' && c != 'R' && c != 'S' && c != 'T' && c != 'U' && c != 'V' && c != 'W' && c != 'X'
        && c != 'Y' && c != 'Z' && c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7'
        && c != '8' && c != '9' && c != '.') {
          tokens.push_back(name);
          name = "";
          IsInName = false;
        } else {
          name += c;
          IsContinued = false;
        }
      } else if (IsInMLComment) {
        if (c == '*') MLCommentEnd = true;
        else if (c != '/') MLCommentEnd = false;
        if (MLCommentEnd && c == '/') {
          IsInMLComment = false;
          MLCommentEnd = false;
        }
        IsContinued = false;
      } else if (IsInComment) {
        if (c == '\n') IsInComment = false;
        IsContinued = false;
      }
      if (IsContinued) {
        toks+=c;
        if (toks == "(" || toks == ")" || toks == "{" || toks == "}") {
          tokens.push_back(toks);
          toks = "";
        } else if (toks == " ") {
          toks = "";
        } else if (toks == "\n") {
          tokens.push_back("\n");
          toks = "";
        } else if (toks == ";") {
          tokens.push_back(";");
          toks = "";
        } else if (toks == ",") {
          tokens.push_back(",");
          toks = "";
        } else if (toks == "\"" || toks == "'" || toks == "`") {
          IsInQuotes = true;
          QuoteInitiator = toks[0];
          quote = "\"";
          toks = "";
        } else if (toks == "0" || toks == "1" || toks == "2" || toks == "3" || toks == "4" || toks == "5" || toks == "6" || toks == "7" || toks == "8" || toks == "9") {
          IsInInt = true;
          number+=toks;
          toks = "";
        } else if (toks == "a" || toks == "b" || toks == "c" || toks == "d" || toks == "e" || toks == "f" || toks == "g" || toks == "h" || toks == "i" || toks == "j"
        || toks == "k" || toks == "l" || toks == "m" || toks == "n" || toks == "o" || toks == "p" || toks == "q" || toks == "r" || toks == "s" || toks == "t"
        || toks == "u" || toks == "v" || toks == "w" || toks == "x" || toks == "y" || toks == "z" || toks == "A" || toks == "B" || toks == "C" || toks == "D"
        || toks == "E" || toks == "F" || toks == "G" || toks == "H" || toks == "I" || toks == "J" || toks == "K" || toks == "L" || toks == "M" || toks == "N"
        || toks == "O" || toks == "P" || toks == "Q" || toks == "R" || toks == "S" || toks == "T" || toks == "U" || toks == "V" || toks == "W" || toks == "X"
        || toks == "Y" || toks == "Z") {
          IsInName = true;
          name+=toks;
          toks = "";
        } else if (toks == "/*") {
          IsInMLComment = true;
          toks = "";
        } else if (toks == "//" || toks == "#") {
          IsInComment = true;
          toks = "";
        }
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