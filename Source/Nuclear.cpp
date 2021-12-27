#include "Nuclear.hpp"
#include <cmath>
#include <algorithm>
#include <map>

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
  Lexer(args->getInput());
  Compiler();
}

void Nuclear::Lexer(std::string path) {
  std::string data = "";
  std::ifstream file(path);
  std::vector<std::string> lines;
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      data += line + "\n";
      lines.push_back(line);
    }
    file.close();
  } else {
    std::cout << "Could not open file '" << path << "'!" << std::endl;
    std::exit(-1);
  }

  std::string toks = "";
  char PreviousChar = '\0';
  bool IsInQuotes = false;
  std::string quote = "";
  char QuoteInitiator = '\0';
  int QuoteLine, QuoteColumn;
  int IsEscaped = 0;
  bool IsInInt = false;
  std::string number = "";
  bool IsInName = false;
  std::string name = "";
  bool IsInMLComment = false;
  bool MLCommentEnd = false;
  bool IsInComment = false;
  bool IsInOperator = false;
  std::string op = "";
  bool IsInMathematicalOperator = false;
  std::string mathop = "";
  
  int line = 1, col = 1, index = 1;

  for (char& c : data) {
    if (IsInQuotes) {
      toks+=c;
      if (c == '\\') {
        IsEscaped++;
        if (IsEscaped == 2) {
          quote += '\\';
          IsEscaped = 0;
        }
      } else if (c == 'n' || c == 't' || c == 'b' || c == 'r') {
        if (PreviousChar == '\\' && IsEscaped == 1) {
          switch (c) {
            case 'n': quote += '\n'; break;
            case 't': quote += '\t'; break;
            case 'b': quote += '\b'; break;
            case 'r': quote += '\r'; break;
          }
          IsEscaped = 0;
        } else quote += c;
      } else if (c == '\n') {
        std::cout << lines[line-1] << std::endl;
        for (unsigned int i=0;i<QuoteColumn-1;i++) std::cout << " ";
        for (unsigned int i=0;i<=lines[line-1].length()-QuoteColumn;i++) std::cout << "^";
        std::cout << std::endl << "Unterminated string at line " << QuoteLine << ", col " << QuoteColumn << "!" << std::endl;
        exit(1);
      } else if (c == '\"' || c == '\'' || c == '`') {
        if (IsEscaped == 1) {
          quote+=c;
          IsEscaped = 0;
        } else if ((c == '\"' && QuoteInitiator == '\"') || (c == '\'' && QuoteInitiator == '\'') || (c == '`' && QuoteInitiator == '`')) {
          quote+='"';
          IsInQuotes = false;
          QuoteInitiator = '\0';
          toks = "";
          Token token = Token(quote, "str", line, col-quote.length()+1);
          tokens.push_back(token);
        } else {
          quote+=c;
        }
      } else quote+=c;
    } else {
      bool IsContinued = true;
      if (IsInInt) {
        if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9' && c != '.') {
          bool isFloat = false, isInt = false, hasDecimal = number.find(".") != std::string::npos;
          if (c == 'd') {
            isFloat = false;
            isInt = false;
            IsContinued = false;
          } else if (c == 'f') {
            isFloat = true;
            isInt = false;
            if (c == 'f') IsContinued = false;
          } else if (hasDecimal) {
            isFloat = false;
            isInt = false;
          } else isInt = true;
          Token token = Token(number, isInt ? "int" : isFloat ? "float" : "double", line, col-number.length());
          tokens.push_back(token);
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
          Token token = Token(name, "name", line, col-name.length());
          tokens.push_back(token);
          name = "";
          IsInName = false;
        } else {
          name += c;
          IsContinued = false;
        }
      } else if (IsInMLComment) {
        if (c == '*') MLCommentEnd = true;
        else if (c != '/') MLCommentEnd = false;
        if (c == '\n') {
          line++;
          col=0;
        }
        if (MLCommentEnd && c == '/') {
          IsInMLComment = false;
          MLCommentEnd = false;
        }
        IsContinued = false;
      } else if (IsInComment) {
        if (c == '\n') {
          line++;
          col=0;
          IsInComment = false;
        }
        IsContinued = false;
      } else if (IsInOperator) {
        if (c != '=' && c != '!' && c != '<' && c != '>' && c != '|' && c != '&') {
          if (!(op == "==" || op == "!=" || op == "!" || op == "<=" || op == ">=" || op == ">" || op == "=" || op == "<" || op == "||" || op == "&&")) {
            std::cout << lines[line-1] << std::endl;
            for (unsigned int i=0;i<col-op.length()-1;i++) std::cout << " ";
            std::cout << "^";
            std::cout << std::endl << "Invalid operator '" << op << "' at line " << line << ", col " << col-op.length() << "!" << std::endl;
            exit(1);
          }
          Token token = Token(op, "op", line, col-op.length()-1);
          tokens.push_back(token);
          op = "";
          IsInOperator = false;
        } else {
          op += c;
          IsContinued = false;
        }
      } else if (IsInMathematicalOperator) {
        mathop+=c;
        if (mathop == "//" || mathop == "/*") {
          IsInMathematicalOperator = false;
          toks+="/";
          IsContinued = true;
          mathop = "";
        } else if (mathop == "+=" || mathop == "-=" || mathop == "*=" || mathop == "/=" || mathop == "%=" || mathop == "++" || mathop == "--") {
          IsInMathematicalOperator = false;
          IsContinued = false;
          Token token = Token(mathop, "mop", line, col-mathop.length()+1);
          tokens.push_back(token);
          mathop = "";
        } else {
          Token token = Token(mathop.substr(0,1), "mop", line, col-1);
          tokens.push_back(token);
          mathop = "";
          IsInMathematicalOperator = false;
          IsContinued = true;
        }
      }
      if (IsContinued) {
        toks+=c;
        if (toks == "(" || toks == ")" || toks == "{" || toks == "}") {
          Token token = Token(toks, "special", line, col-toks.length()+1);
          tokens.push_back(token);
          toks = "";
        } else if (toks == " " || toks == "\t") {
          toks = "";
        } else if (toks == "\n") {
          line++;
          col = 0;
          Token token = Token(toks, "special", line, col-toks.length()+1);
          tokens.push_back(token);
          toks = "";
        } else if (toks == ";") {
          Token token = Token(toks, "special", line, col-toks.length()+1);
          tokens.push_back(token);
          toks = "";
        } else if (toks == ",") {
          Token token = Token(toks, "special", line, col-toks.length()+1);
          tokens.push_back(token);
          toks = "";
        } else if (toks == "\"" || toks == "'" || toks == "`") {
          IsInQuotes = true;
          QuoteLine = line;
          QuoteColumn = col;
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
        || toks == "Y" || toks == "Z" || toks == "@") {
          IsInName = true;
          name+=toks;
          toks = "";
        } else if (toks == "/*") {
          IsInMLComment = true;
          toks = "";
        } else if (toks == "//" || toks == "#") {
          IsInComment = true;
          toks = "";
        } else if (toks == "=" || toks == "!" || toks == "<" || toks == ">" || toks == "|" || toks == "&") {
          IsInOperator = true;
          op+=toks;
          toks = "";
        } else if (toks == "+" || toks == "-" || toks == "*" || toks == "/" || toks == "%" || toks == "^") {
          IsInMathematicalOperator = true;
          mathop+=toks;
          toks = "";
        } else if (toks == "[" || toks == "]") {
          Token token = Token(toks, "special", line, col-toks.length()+1);
          tokens.push_back(token);
          toks = "";
        } else {
          std::cout << lines[line-1] << std::endl;
          for (unsigned int i=0;i<col-1;i++) std::cout << " ";
          std::cout << "^";
          std::cout << std::endl << "Invalid token '" << toks.substr(0, 1) << "' at line " << line << ", col " << col-toks.length()+1 << "!" << std::endl;
          exit(1);
        }
      }
    }
    PreviousChar = c;
    col++;
    index++;
    if (tokens.size() >= 2 && tokens[tokens.size()-2].getValue() == "import") {
      if (tokens[tokens.size()-1].getType() == "str") {
        std::string file = tokens[tokens.size()-1].getValue().substr(1, tokens[tokens.size()-1].getValue().length()-2);
        for (std::string import: imports) {
          if (import == file) {
            std::cout << lines[tokens[tokens.size()-2].getLine()-1] << std::endl;
            for (unsigned int i=0;i<tokens[tokens.size()-2].getColumn()-1;i++) std::cout << " ";
            for (unsigned int i=0;i<tokens[tokens.size()-2].getValue().length();i++) std::cout << "^";
            std::cout << std::endl << "Bad import at line " << tokens[tokens.size()-2].getLine() << ", col " << tokens[tokens.size()-2].getColumn() << ", in '" << path << "'!" << std::endl;
            std::cout << "Import '" << file << "' already imported!" << std::endl;
            exit(1);
          }
        }
        imports.push_back(file);
        Token token = Token("", "blank", line, col);
        tokens[tokens.size()-1] = token;
        tokens[tokens.size()-2] = token;
        Lexer(file);
      } else {
          std::cout << lines[tokens[tokens.size()-2].getLine()-1] << std::endl;
          for (unsigned int i=0;i<tokens[tokens.size()-2].getColumn()-1;i++) std::cout << " ";
          for (unsigned int i=0;i<tokens[tokens.size()-2].getValue().length();i++) std::cout << "^";
          std::cout << std::endl << "Please include a string after include at line " << tokens[tokens.size()-2].getLine() << ", col " << tokens[tokens.size()-2].getColumn() << "!" << std::endl;
          exit(1);
      }
    } 
  }
  for (int i=0;i<tokens.size();i++) if (tokens[i].getType() == "blank" && tokens[i].getValue() == "") tokens.erase(tokens.begin()+i-1);
}

void Nuclear::Compiler() {
  int ExitCode = 0;
  bool HasExited = false;
  std::vector<std::vector<std::string>> assembly;
  std::vector<std::string> text, data;
  data.push_back("section .data");
  text.push_back("section .text");
  text.push_back("global _start");
  text.push_back("_start:");
  for (unsigned int a=0;a<tokens.size();a++) {
    Token token = tokens[a];
    // std::cout << token.getValue() << std::endl;
    if (token.getValue() == "print" && tokens[a+1].getValue() == "(" && tokens[a+2].getType() == "str" && tokens[a+3].getValue() == ")") {
      std::string string = tokens[a+2].getValue().substr(1, tokens[a+2].getValue().length()-2);
      while (string.find("\n") != std::string::npos) {
        string.replace(string.find("\n"), std::string("\n").size(), "\\n");
      }
      while (string.find("\t") != std::string::npos) {
        string.replace(string.find("\t"), std::string("\t").size(), "\\t");
      }
      while (string.find("\b") != std::string::npos) {
        string.replace(string.find("\b"), std::string("\b").size(), "\\b");
      }
      while (string.find("\r") != std::string::npos) {
        string.replace(string.find("\r"), std::string("\r").size(), "\\r");
      }
      data.push_back("str" + std::to_string(a) + ": db '" + string + "', 10");
      data.push_back("strlen" + std::to_string(a) + ": equ $-str" + std::to_string(a));
      text.push_back("  mov edx, strlen" + std::to_string(a));
      text.push_back("  mov ecx, str" + std::to_string(a));
      text.push_back("  mov ebx, 1");
      text.push_back("  mov eax, 4");
      text.push_back("  int 0x80");
    } else if (token.getValue() == "return" || token.getValue() == "exit") {
      ExitCode = tokens[a+1].getType()=="int"?ExitCode=std::stoi(tokens[a+1].getValue()):0;
      HasExited = true;
      text.push_back("  mov eax, 1");
      text.push_back("  mov ebx, " + std::to_string(ExitCode));
      text.push_back("  int 0x80");
      break;
    }
  }

  if (!HasExited) {
    text.push_back("  ");
    text.push_back("  mov eax, 1");
    text.push_back("  mov ebx, " + std::to_string(ExitCode));
    text.push_back("  int 0x80");
  }

  assembly.push_back(data);
  assembly.push_back(text);

  for (std::vector<std::string> seg: assembly) {
    for (std::string str: seg) {
      std::cout << str << std::endl;
    }
  }
}