#pragma once

#include <queue>
#include <string>

typedef int Cell;

struct CellID {
    int x, y, dist;
    CellID() {};
    CellID(int x, int y, int dist);
};

enum Check {
    CHECK_UP = 0,
    CHECK_RIGHT,
    CHECK_DOWN,
    CHECK_LEFT,
};

const Cell CELL_BLOCK   = -1;
const Cell CELL_START   = -2;
const Cell CELL_END     = -3;
const Cell CELL_FINISH  = -4;

class Grid {
public:
    Grid(int w, int l);
    ~Grid();

    void setStart(int x, int y);
    void setEnd(int x, int y);
    void simulate();
    bool simulateStep();
    void printGrid();
    std::string printSymbol(Cell val);
    bool checkCell(int x, int y, Check check);
    bool checkValue(Cell val);
    void setBlock(int x, int y);
    void calculatePath();
private:
    Cell **grid_;

    std::queue<CellID> visiting_;

    int width_;
    int length_;

    int xstart_;
    int ystart_;

    int xend_;
    int yend_;
};
