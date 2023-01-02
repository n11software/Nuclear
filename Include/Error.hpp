#pragma once
#include <Position.hpp>
#include <string>

class Error {
    public:
        Error();
        void SetPosition(Position start);
        void SetPosition(Position start, Position end);
        void SetPath(std::string path);
        void SetMessage(std::string message);
        void SetType(int level);
        void Print();
    private:
        Position start, end;
        std::string path, message;
        int level;
};