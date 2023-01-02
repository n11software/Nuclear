#include <Token.hpp>

Token::Token() {}

void Token::SetPosition(Position start, Position end) {
    this->start = start;
    this->end = end;
}

void Token::SetType(std::string type) {
    this->type = type;
}

void Token::SetValue(std::string value) {
    this->value = value;
}

std::vector<Position> Token::GetPosition() {
    return {start, end};
}

std::string Token::GetType() {
    return type;
}

std::string Token::GetValue() {
    return value;
}