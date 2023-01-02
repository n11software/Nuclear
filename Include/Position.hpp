#pragma once

class Position {
    public:
        Position();
        Position(int line, int column);
        int GetLine();
        int GetColumn();
    private:
        int line, column;
};