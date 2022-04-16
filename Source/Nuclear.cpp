/*
 * Nuclear.cpp - Welcome to my absolute mess.
 *
 *  ###########################################################
 *                    WARNING: ESSAY INCOMING
 *  ###########################################################
 *
 *  This file will be likely split into more files soon but for 
 *  now this is where the heart of the program is, this file will 
 *  attempt to take the arguments from main.cpp and parse them and 
 *  send them back to main.cpp and it will do a few checks and send 
 *  the arguments straight back here for the lexer. After that whole
 *  mess the lexer will take all of the characters in the code that
 *  is being compiled and make it more readable for the compiler.
 *  Then it will take that and construct assembly code and put that into
 *  a temperary file and make fasm assemble it (Hopefully will be replaced
 *  with a custom assembler in the future).
 */

#include "Nuclear.hpp"
#include <cmath>
#include <algorithm>
#include <map>
#include <time.h>
#include <random>
#include <cctype>
#include <sys/stat.h>
#include <filesystem>

/*
 * Shows an error and exits
 *
 * @param msg Provides the error message (Duh)
 * @param col I kinda forgot why we need two of these...
 * @param line Well where was it????
 * @param colStart Where on the line did it end?
 * @param colEnd Where exactly on the line did it end?
 */
void Error(std::string msg, int col, int line, int colStart, int colEnd, std::string lineData, std::string file) {
  fprintf(stderr, "%s\n", lineData.c_str());
  for (unsigned int i=0;i<col-colStart;i++) fprintf(stderr, " ");
  fprintf(stderr, "\033[31m");
  for (unsigned int i=0;i<colEnd;i++) fprintf(stderr, "^");
  fprintf(stderr, "\n\033[1m\033[31m%s at line %i, col %i, in file %s!\033[0m\n", msg.c_str(), line, col, file.c_str());
  exit(1);
}

/*
 * I kinda got sick of typing so...
 * 
 * @param str If you can't guess what 
 * to put here just go...
 * @return The same string but lowercase.
 */
std::string toLower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

/*
 * This will parse all the arguments.
 * 
 * @param argc Count of arguments.
 * @param argv Array of arguments.
 */
Arguments::Arguments(int argc, char** argv) {
  // This whole function will likely be replaced later
  // for a more flexible version.

  // Just in case we will store the original parameters.
  this->argc = argc;
  this->argv = argv;
  if (argc == 1) {
    // To be changed later.
    std::cout << "Nuclear v" << Version << std::endl;
    exit(0);
  }
  for (unsigned int a=0;a<argc;a++) {
    if (std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v") {
      // Seriously take a guess on what this does.
      std::cout << "Nuclear v" << Version << std::endl;
      exit(0);
    }
    // Shows the time taken for the full compilation.
    if (std::string(argv[a]) == "--time" || std::string(argv[a]) == "-t") {showTime = true;continue;}
    // This will run the output binary directly after compilation.
    if (std::string(argv[a]) == "--run" || std::string(argv[a]) == "-r") {runAfter = true;continue;}
    // This will just put the assembly into the output file.
    if (std::string(argv[a]) == "--assembly" || std::string(argv[a]) == "--asm" || std::string(argv[a]) == "-S") {outputAssembly = true;continue;}
    // Everything below is for parsing inputs and outputs.
    std::string extension = std::string(argv[a]).substr(std::string(argv[a]).find_last_of(".") + 1); // find the file extension
    if (extension == std::string(argv[a])) extension = "";
    if (extension == "" || extension == "exe") {
      this->output = std::string(argv[a]);
    } else if (extension == toLower("n") || extension == toLower("nuclear") || extension == toLower("nuke")) {
      // Check the file's existence
      if (!std::ifstream(argv[a]).good()) {
        fprintf(stderr, "Error: File %s does not exist!\n", argv[a]);
        exit(0);
      }
      // If the file is named [fileName].(N|Nuclear)
      // then the default library will be disabled
      // e.g. (print, files, etc..)
      if (extension.substr(0, 1) == "N") {
        this->input = std::string(argv[a]);
        isLibEnabled = false;
      } else {
        this->input = std::string(argv[a]);
        isLibEnabled = true;
      }
    }
  }
}

/*
 * Now that the nonsense is over
 * we can continue
 * 
 * @param args Dude I kinda need to
 * know what's going on here...
 */
Nuclear::Nuclear(Arguments* args) {
  this->args = args;
  Lexer(args->getInput());
  Compiler();
}

/*
 * Massive amounts of code that
 * just over-simplifies the file's code.
 * 
 * @param path The path to the source code.
 */
void Nuclear::Lexer(std::string path) {
  // Check the file and read it into data
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
    this->paths.push_back(path);
    this->lines.push_back(lines);
  } else {
    fprintf(stderr, "Could not open file '%s'!\n", path.c_str());
    std::exit(-1);
  }

  // These are needed to check the status of the lexer
  // you'll understand in a bit.
  std::string toks = "";
  char PreviousChar = '\0';
  bool IsInQuotes = false;
  std::string quote = "";
  char QuoteInitiator = '\0';
  int QuoteLine, QuoteColumn;
  int IsEscaped = 0, EscapedCol = 0;
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

  // Loop through every single character in the source code.
  // I recommend to start at [Reference #00]
  for (char& c : data) {
    if (IsInQuotes) { // Reference #01
      toks+=c;
      if (c == '\\') {
        // If the character is \ then we wait until 
        // the next loop to see what type of escape it is.
        IsEscaped++;
        EscapedCol=col-1;
        if (IsEscaped == 2) { // If it is "\\"
          // If it came back here and there had already been an 
          // escape character then we know they just want to 
          // have a \ in the string
          quote += '\\';
          IsEscaped = 0;
        }
      } else if (c == 'n' || c == 't' || c == 'b' || c == 'r') {
        // Now that I think of it, it's a bit of an odd way of doing this.
        // But we check if it's escaped then we add the correlating escape character.
        if (PreviousChar == '\\' && IsEscaped == 1) {
          switch (c) {
            case 'n': quote += '\n'; break;
            case 't': quote += '\t'; break;
            case 'b': quote += '\b'; break;
            case 'r': quote += '\r'; break;
          }
          IsEscaped = 0;
        } else quote += c; // If it wasn't escaped just add it to the string.
      } else if (c == '\n') Error("Unterminated string", QuoteColumn, QuoteLine, 0, lines[line-1].length()-QuoteColumn, lines[QuoteLine-1], path); // Dude seriously you need to learn to close your strings.
      else if (c == '\"' || c == '\'' || c == '`') {
        // Not entirely sure how this error would occur. (I forgot)
        if (IsEscaped == 1) Error("Unknown terminator", EscapedCol, line, 0, 2, lines[line-1], path);
        else if ((c == '\"' && QuoteInitiator == '\"') || (c == '\'' && QuoteInitiator == '\'') || (c == '`' && QuoteInitiator == '`')) {
          // Well it seems that your massive essay in those quotes is finally finished and you remembered that strings need two quotes.
          // We'll add the end quotes to your string and get out of this horrid loop.
          // And while we're at it will add your string to the tokens array and reset everything.
          quote+=QuoteInitiator;
          IsInQuotes = false;
          QuoteInitiator = '\0';
          toks = "";
          Token token = Token(quote, "str", line, col-quote.length()+1, path);
          tokens.push_back(token);
        }
      } else quote+=c; // For literally every other character that is a part of your essay.
    } else {
      // We hadn't really introduced this before but
      // IsContinued will just allow the main loop [Reference #00] to continue
      bool IsContinued = true;
      if (IsInInt) { // Reference #02
        if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9' && c != '.') {
          // If the character is anything other than a number 
          // then we'll figure out what the hell to do with it.
          bool isFloat = false, isInt = false, hasDecimal = number.find(".") != std::string::npos;
          if (c == 'd') {
            // Why? not sure...
            isFloat = false;
            isInt = false;
            IsContinued = false;
          } else if (c == 'f') {
            // Was I thinking when I made this?????
            isFloat = true;
            isInt = false;
            if (c == 'f') IsContinued = false; // IT'S ALWAYS TRUE!! WHYYYYYYYY??? IDIOT.
          } else if (hasDecimal) {
            isFloat = false; // Shouldn't this be true???
            isInt = false;
          } else isInt = true;
          // Whatever no matter how stupid it is, it somehow managed to float past my smol brain
          // I will fix this later. - FiRe
          Token token = Token(number, isInt ? "int" : isFloat ? "float" : "double", line, col-number.length(), path);
          tokens.push_back(token);
          number = "";
          IsInInt = false; // Get out of this mess of code.
        } else {
          // It's a number so we'll just continue to add it to our string and loop back.
          number += c;
          IsContinued = false;
        }
      } else if (IsInName) { // Reference #03
        if (c != 'a' && c != 'b' && c != 'c' && c != 'd' && c != 'e' && c != 'f' && c != 'g' && c != 'h' && c != 'i' && c != 'j'
        && c != 'k' && c != 'l' && c != 'm' && c != 'n' && c != 'o' && c != 'p' && c != 'q' && c != 'r' && c != 's' && c != 't'
        && c != 'u' && c != 'v' && c != 'w' && c != 'x' && c != 'y' && c != 'z' && c != 'A' && c != 'B' && c != 'C' && c != 'D'
        && c != 'E' && c != 'F' && c != 'G' && c != 'H' && c != 'I' && c != 'J' && c != 'K' && c != 'L' && c != 'M' && c != 'N'
        && c != 'O' && c != 'P' && c != 'Q' && c != 'R' && c != 'S' && c != 'T' && c != 'U' && c != 'V' && c != 'W' && c != 'X'
        && c != 'Y' && c != 'Z' && c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7'
        && c != '8' && c != '9' && c != '.') {
          // Since it's not a letter then we'll just consider the name finished and add it to the tokens.
          Token token = Token(name, "name", line, col-name.length(), path);
          tokens.push_back(token);
          name = "";
          IsInName = false;
        } else {
          // Add the character to the name and loop back.
          name += c;
          IsContinued = false;
        }
      } else if (IsInMLComment) { // Reference #04
        // My brain can't understand this right now 
        // but it's not nessisarily important anyways 
        // if you'd like to read it and add comments have at it.
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
      } else if (IsInComment) { // Reference #05
        // Probably the most simple part of this whole compiler.
        if (c == '\n') {
          line++;
          col=0;
          IsInComment = false;
        }
        IsContinued = false;
      } else if (IsInOperator) { // Reference #06
        if (c != '=' && c != '!' && c != '<' && c != '>' && c != '|' && c != '&') {
          if (!(op == "==" || op == "!=" || op == "!" || op == "<=" || op == ">=" || op == ">" || op == "=" || op == "<" || op == "||" || op == "&&")) Error("Invalid operator", col-op.length(), line, 1, op.length(), lines[line-1], path);
          Token token = Token(op, "op", line, col-op.length()-1, path);
          tokens.push_back(token);
          op = "";
          IsInOperator = false;
        } else {
          op += c;
          IsContinued = false;
        }
      } else if (IsInMathematicalOperator) { // Reference #07
        mathop+=c;
        if (mathop == "//" || mathop == "/*") {
          // This will put the lexer on the right track and move to the comment lexer.
          IsInMathematicalOperator = false;
          toks+="/";
          IsContinued = true;
          mathop = "";
        } else if (mathop == "+=" || mathop == "-=" || mathop == "*=" || mathop == "/=" || mathop == "%=" || mathop == "++" || mathop == "--") {
          // Honestly looking at this now I think it would have somewhat of a stroke from this.
          // I'll get to it when it comes up. If it does...
          IsInMathematicalOperator = false;
          IsContinued = false;
          Token token = Token(mathop, "mop", line, col-mathop.length()+1, path);
          tokens.push_back(token);
          mathop = "";
        } else {
          // I think these are a bit swapped or something.
          Token token = Token(mathop.substr(0,1), "mop", line, col-1, path);
          tokens.push_back(token);
          mathop = "";
          IsInMathematicalOperator = false;
          IsContinued = true;
        }
      }
      if (IsContinued) { // Ignore this for now if you're coming from the start
        // Reference #00
        toks+=c; // toks just keeps track of the current array of characters that are important.
        if (toks == "(" || toks == ")" || toks == "{" || toks == "}") {
          // if we see one of these characters we will add it to the tokens array and reset 
          // toks because the character is no longer important
          Token token = Token(toks, "special", line, col-toks.length()+1, path);
          tokens.push_back(token);
          toks = "";
        } else if (toks == " " || toks == "\t") {
          toks = "";
        } else if (toks == "\n") {
          line++;
          col = 0;
          Token token = Token(toks, "special", line, col-toks.length()+1, path);
          tokens.push_back(token);
          toks = "";
        } else if (toks == ";") {
          Token token = Token(toks, "special", line, col-toks.length()+1, path);
          tokens.push_back(token);
          toks = "";
        } else if (toks == ",") {
          Token token = Token(toks, "special", line, col-toks.length()+1, path);
          tokens.push_back(token);
          toks = "";
        } else if (toks == "\"" || toks == "'" || toks == "`") {
          // If we see a quotation we need to set IsInQuotes to true to ensure that next 
          // time we loop we'll enter the if statement at line [Reference #01].
          IsInQuotes = true;
          // We need to keep track of this for error messages.
          QuoteLine = line;
          QuoteColumn = col;
          // We need to keep track of what we used to open the string so we 
          // can only close it with that specific character.
          QuoteInitiator = toks[0];
          quote = "\"";
          toks = "";
        } else if (toks == "0" || toks == "1" || toks == "2" || toks == "3" || toks == "4" || toks == "5" || toks == "6" || toks == "7" || toks == "8" || toks == "9") {
          // Same thing as before but we use line [Reference #02] instead.
          IsInInt = true;
          // Deposit current important characters into the numbers variable.
          number+=toks;
          toks = "";
        } else if (toks == "a" || toks == "b" || toks == "c" || toks == "d" || toks == "e" || toks == "f" || toks == "g" || toks == "h" || toks == "i" || toks == "j"
        || toks == "k" || toks == "l" || toks == "m" || toks == "n" || toks == "o" || toks == "p" || toks == "q" || toks == "r" || toks == "s" || toks == "t"
        || toks == "u" || toks == "v" || toks == "w" || toks == "x" || toks == "y" || toks == "z" || toks == "A" || toks == "B" || toks == "C" || toks == "D"
        || toks == "E" || toks == "F" || toks == "G" || toks == "H" || toks == "I" || toks == "J" || toks == "K" || toks == "L" || toks == "M" || toks == "N"
        || toks == "O" || toks == "P" || toks == "Q" || toks == "R" || toks == "S" || toks == "T" || toks == "U" || toks == "V" || toks == "W" || toks == "X"
        || toks == "Y" || toks == "Z" || toks == "@") {
          // This is just a very slow way of checking for variables or functions.
          IsInName = true; // [Reference #03]
          name+=toks;
          toks = "";
        } else if (toks == "/*") {
          IsInMLComment = true; // [Reference #04]
          toks = "";
        } else if (toks == "//" || toks == "#") {
          IsInComment = true; // [Reference #05]
          toks = "";
        } else if (toks == "=" || toks == "!" || toks == "<" || toks == ">" || toks == "|" || toks == "&") {
          IsInOperator = true; // [Reference #06]
          op+=toks;
          toks = "";
        } else if (toks == "+" || toks == "-" || toks == "*" || toks == "/" || toks == "%" || toks == "^") {
          IsInMathematicalOperator = true; // [Reference #07]
          mathop+=toks;
          toks = "";
        } else if (toks == "[" || toks == "]") {
          // not sure why we didn't put this with () and {} but I'll move it later.
          Token token = Token(toks, "special", line, col-toks.length()+1, path);
          tokens.push_back(token);
          toks = "";
        } else Error("Invalid token", col, line, 1, 1, lines[line-1], path); // and of course you had to put some random character we haven't even heard of!
      }
    }
    PreviousChar = c; // Just in case for the next loop.
    col++;
    index++;
    // This will check for an import of another source file 
    // and then just do this same function but with that file.
    if (tokens.size() >= 2 && tokens[tokens.size()-2].getValue() == "import") {
      if (tokens[tokens.size()-1].getType() == "str") {
        std::string file = tokens[tokens.size()-1].getValue().substr(1, tokens[tokens.size()-1].getValue().length()-2);
        for (std::string import: imports) if (import == file) Error("Import loop", tokens[tokens.size()-1].getColumn(), tokens[tokens.size()-1].getLine(), 0, tokens[tokens.size()-1].getValue().length()-2, lines[tokens[tokens.size()-1].getLine()-1], path);
        imports.push_back(file);
        Token token = Token("", "blank", line, col, path);
        tokens[tokens.size()-1] = token;
        tokens[tokens.size()-2] = token;
        Lexer(file);
      } else Error("Please include a string after include", tokens[tokens.size()-2].getColumn(), tokens[tokens.size()-2].getLine(), 1, tokens[tokens.size()-2].getValue().length(), lines[tokens[tokens.size()-2].getLine()-1], path);
    } 
  }
  // Remove pesky blank tokens
  for (int i=0;i<tokens.size();i++) if (tokens[i].getType() == "blank" && tokens[i].getValue() == "") tokens.erase(tokens.begin()+i-1);
}

/*
 * Replace All and Swap. 
 * Basically just replaces every instance of a string to something else.
 * 
 * @param source The haystack.
 * @param from The needle.
 * @param to The new needle?
 */
void RAS(std::string& source, const std::string& from, const std::string& to) {
  std::string newString;
  newString.reserve(source.length());
  std::string::size_type lastPos = 0;
  std::string::size_type findPos;
  while (std::string::npos != (findPos = source.find(from, lastPos))) {
    newString.append(source, lastPos, findPos - lastPos);
    newString += to;
    lastPos = findPos + from.length();
  }
  newString += source.substr(lastPos);
  source.swap(newString);
}

/*
 * This will take all the tokens and turn them into assembly code
 */
void Nuclear::Compiler() {
  int ExitCode = 0;
  bool HasExitted = false;
  // These arrays will be put together later but it's here to organize the code properly.
  std::vector<std::vector<std::string>> assembly;
  std::vector<std::string> text, data, global;
  std::vector<Function> functions;
  if (args->getIsLibEnabled()) {
    Function print;
    print.name = "print";
    functions = {print};
  }
  // Basic header information
  global.push_back("format ELF64 executable 3");
  data.push_back("segment readable writable");
  text.push_back("segment readable executable");
  text.push_back("entry _start");

  text.push_back("_start:");
  for (unsigned int a=0;a<tokens.size();a++) {
    // Loop through all the tokens
    Token token = tokens[a];
    if (tokens.size() >= a+3 && token.getType() == "name" && tokens[a+1].getType() == "special" && tokens[a+1].getValue() == "(") {
      // Parse functions

      // Get how many parameters were passed through
      int arguments = -1;
      for (int x=0;x<tokens.size();x++) if (tokens[x].getType() == "special" && tokens[x].getValue() == ")") {
        if (a+1-x != 0) arguments = -(a+1-x)/2;
        else arguments = 0;
        break;
      }

      int y=0;
      for (y;y<paths.size();y++) if (paths[y] == tokens[a+1].getPath()) break;
      if (lines[y][tokens[a].getLine()-1].find("{") != std::string::npos) {
        // This is not fully implemented but it will allow you to 
        // create new functions.
        for (Function func: functions) if (func.name == token.getValue()) Error("Redefinition of function '" + token.getValue() + "'", tokens[a].getColumn(), tokens[a].getLine(), 1, tokens[a].getValue().length(), lines[y][tokens[a].getLine()-1], paths[y]);
        Function x;
        x.name = token.getValue();
        functions.push_back(x);
        int z=0;
        for (z=0;z<tokens.size();z++) if (tokens[z].getValue() == "}") break;
        tokens.erase(std::next(tokens.begin(), a), std::next(tokens.begin(), z+1));
        continue;
      }
      for (Function func: functions) if (func.name != token.getValue()) {
        // This function has never been declared.
        int y=0;
        for (y;y<paths.size();y++) if (paths[y] == tokens[a].getPath()) break;
        Error("Undefined function", tokens[a].getColumn(), tokens[a].getLine(), 1, tokens[a].getValue().length(), lines[y][tokens[a].getLine()-1], paths[y]);
      } else {
        if (arguments == -1) {
          // Please be smart and add a closing '}'...
          int y=0;
          for (y;y<paths.size();y++) if (paths[y] == tokens[a+1].getPath()) break;
          Error("Unterminated function", tokens[a+1].getColumn(), tokens[a+1].getLine(), 1, tokens[a+1].getValue().length(), lines[y][tokens[a+1].getLine()-1], paths[y]);
        }

        // Can't remember why I did this twice. 
        // I'm sure there's a stupid reason.
        int start = a+1;
        for (int x=0;x<tokens.size();x++) if (tokens[x].getType() == "special" && tokens[x].getValue() == ")") {
          if (a+1-x != 0) start = -(a+2-x);
          else start = 0;
          break;
        }

        if (token.getValue() == "print") {
          // Implement the print function (Only works on Linux for now).
          for (int x=0;x<start;x++) {
            // Ensure that there is a comma in between each parameter.
            if (tokens[a+1+(x*2)+2].getValue() != "," && tokens[a+1+(x*2)+2].getValue() != ")") {
              int y=0;
              for (y;y<paths.size();y++) if (paths[y] == tokens[a+1+(x*2)+1].getPath()) break;
              Error("Expected a ','", tokens[a+1+(x*2)+1].getColumn()+tokens[a+1+(x*2)+1].getValue().length(), tokens[a+1+(x*2)+1].getLine(), 1, 2, lines[y][tokens[a+1+(x*2)+1].getLine()-1], paths[y]);
            }
          }
          // If there are no parameters what the hell are we going to print???
          if (arguments == 0) continue;

          int len = 0;
          std::string out = "";
          // Loop through each argument 
          for (int x=0;x<arguments;x++) {
            if (!(tokens[a+1+(x*2)+1].getType()=="str" || tokens[a+1+(x*2)+1].getType()=="int" || tokens[a+1+(x*2)+1].getType()=="double" || tokens[a+1+(x*2)+1].getType()=="float")) {
              // Dude you can't pass a class with print, you get an F.
              int y=0;
              for (y;y<paths.size();y++) if (paths[y] == tokens[a+1+(x*2)+1].getPath()) break;
              Error("Expected a string, int, double or float", tokens[a+1+(x*2)+1].getColumn(), tokens[a+1+(x*2)+1].getLine(), 1, tokens[a+1+(x*2)+1].getValue().length(), lines[y][tokens[a+1+(x*2)+1].getLine()-1], paths[y]);
            }
            std::string value = "";
            if (tokens[a+1+(x*2)+1].getValue().substr(0,1) == "\"") value = tokens[a+1+(x*2)+1].getValue().substr(1, tokens[a+1+(x*2)+1].getValue().length()-2);
            else value = tokens[a+1+(x*2)+1].getValue();
            len+=value.length();
            // Replace special characters with assembly safe ones.
            RAS(value, "'", "', 39, '");
            while (value.find("\n") != std::string::npos) value.replace(value.find("\n"), std::string("\n").size(), "', 10, '");
            while (value.find("\t") != std::string::npos) value.replace(value.find("\t"), std::string("\t").size(), "', 9, '");
            std::string end = "', 10, 0";
            if (x!=arguments-1) {
              end = " ', '";
              len++;
            } else len+=2;
            out += value + end;
          }
          // Add the string to the .data section
          data.push_back("str" + std::to_string(a) + " db '" + out);
          // Add the print code to .text
          text.push_back("  lea rdi, [str" + std::to_string(a) + "]");
          text.push_back("  mov rax, " + std::to_string(len));
          text.push_back("  mov rdx, rax");
          text.push_back("  mov rsi, rdi");
          text.push_back("  mov rax, 1");
          text.push_back("  mov rdi, 1");
          text.push_back("  syscall");
        }
        // Get rid of this function to reduce headache.
        tokens.erase(std::next(tokens.begin(), a), std::next(tokens.begin(), a+arguments*2+2));
      }
    } else if (token.getValue() == "return" || token.getValue() == "exit") {
      // Are you supprised that we have more than a print function?
      ExitCode = tokens[a+1].getType()=="int"?ExitCode=std::stoi(tokens[a+1].getValue()):0;
      HasExitted = true;
      text.push_back("  mov rdi, " + std::to_string(ExitCode));
      text.push_back("  mov rax, 60");
      text.push_back("  syscall");
      break;
    }
  }

  if (!HasExitted) {
    // If manually exitted then we'll do so.
    text.push_back("  ");
    text.push_back("  mov rdi, " + std::to_string(ExitCode));
    text.push_back("  mov rax, 60");
    text.push_back("  syscall");
  }

  // Combine all the arrays
  assembly.push_back(global);
  assembly.push_back(data);
  assembly.push_back(text);
  
  // Generate a random string to protect against 
  // collision between two files named the same thing.
  std::string r = "";
  if (args->getOutputAssembly()) {
    std::random_device OSSeed;
    const uint_least32_t seed = OSSeed();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<uint_least32_t> distribute(0, 9);
    for (int repetition=0;repetition<10;repetition++) r+=std::to_string(distribute(generator));
  }

  // Create an actual string from the assembly array
  std::string Output;
  for (std::vector<std::string> seg: assembly) {
    for (std::string str: seg) {
      Output+=str+'\n';
    }
  }

  // Write the assembly to a file in /tmp/ for fasm
  // unless if we have -S then we will just put it as the output file.
  std::fstream file;
  file.open(args->getOutputAssembly() ? args->getOutput() : "/tmp/"+args->getOutput()+"-"+r+".S", std::fstream::out);
  file << Output;
  file.close();

  // Now we kindly ask fasm to do the goddamn work for us.
  if (!args->getOutputAssembly()) {
    system(("fasm /tmp/"+args->getOutput()+"-"+r+".S "+args->getOutput()+" > /dev/null").c_str());
    chmod(args->getOutput().c_str(), S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IXUSR|S_IXGRP|S_IXOTH);

    // Delete the temp file.
    if (remove(("/tmp/"+args->getOutput()+"-"+r+".S").c_str()) != 0) {
      fprintf(stderr, "Error whilst removing temporary file for FASM!\n");
      exit(1);
    }
  }
}