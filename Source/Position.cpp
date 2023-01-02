#include <Position.hpp>

Position::Position() {
    this->line = 0;
    this->column = 0;
}

Position::Position(int line, int column) {
    this->line = line;
    this->column = column;
}

int Position::GetLine() {
    return this->line;
}

int Position::GetColumn() {
    return this->column;
}