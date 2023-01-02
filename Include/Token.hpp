#pragma once
#include <string>
#include <Position.hpp>
#include <vector>

class Token {
    public:
        Token();
        void SetPosition(Position start, Position end);
        void SetType(std::string type);
        void SetValue(std::string value);
        std::vector<Position> GetPosition();
        std::string GetType();
        std::string GetValue();
    private:
        Position start, end;
        std::string type, value;
};