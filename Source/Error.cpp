#include <Error.hpp>
#include <iostream>
#include <fstream>

Error::Error() {

}

void Error::SetPosition(Position start) {
    this->start = start;
}

void Error::SetPosition(Position start, Position end) {
    this->start = start;
    this->end = end;
}

void Error::SetPath(std::string path) {
    this->path = path;
}

void Error::SetMessage(std::string message) {
    this->message = message;
}

void Error::SetType(int level) {
    this->level = level;
}

void Error::Print() {
    std::string color = "\033[1;37m";
    std::string type = "Info";
    switch (level) {
        case 1:
            color = "\033[1;33m";
            type = "Warning";
            break;
        case 2:
            color = "\033[1;31m";
            type = "Error";
            break;
    }

    if (path != "") {
        std::string output;
        std::ifstream file(path);
        if (file.is_open()) {
            int StartCol = start.GetColumn()-1;
            int StartLine = start.GetLine();
            int EndCol = end.GetColumn()-1;
            int EndLine = end.GetLine();
            std::string f;
            for (int i = 0; i < StartLine; i++) std::getline(file, output);
            if (output.size() <= 0) return;
            if (StartCol > output.size()) StartCol = output.size();
            if (EndCol > output.size()) EndCol = output.size();
            f += output.substr(0, StartCol);
            f += color + "\033[4m" + output.substr(StartCol, EndCol-StartCol) + "\033[0m";
            if (end.GetColumn() != 0) f += output.substr(EndCol, output.length());
            f += "\033[0m\n";
            for (int i = 0; i < StartCol; i++) f += " ";
            if (EndCol == 0) {
                for (int i = StartCol; i < output.length(); i++) f += color + "^";
            } else {
                for (int i = StartCol; i < EndCol; i++) f += color + "^";
            }
            f += "\033[0m\n";
            f += "At " + path + ":" + std::to_string(StartLine) + ":" + std::to_string(StartCol+1) + "\033[0m\n";
            f += color + type + "\033[0m: " + message;

            if (level == 2) std::cerr << f << std::endl;
            else std::cout << f << std::endl;
            return;
        }
        
        Error error;
        error.SetMessage("Could not open file!");
        error.SetType(2);
        error.Print();
    } else {
        std::cout << color << type << "\033[0m: " << message << std::endl;
    }
}