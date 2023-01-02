#pragma once
#include <string>
#include <Token.hpp>

class Variable {
    public:
        Variable();
        Variable(std::string name);
        std::string GetName();
        Token value;
    private:
        std::string name;
};