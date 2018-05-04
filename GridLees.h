#pragma once

#include <queue>
#include <string>
#include "GridBase.h"

typedef int Cell;

const Cell CELL_PIN		= -1;
const Cell CELL_EMPTY	= -2;
const Cell CELL_BLOCK	= -3;

class GridLees {
public:
	GridLees(unsigned int width, unsigned int length);
    ~GridLees();

	void setBlockers(unsigned int size, Coord *coordinates);
	void addPath(int xs, int ys, int xe, int ye);
	void addPath(Coord start, Coord end);
	void addPath(Path path);
    void simulate();
private:
    Cell *grid_;

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

    unsigned int width_;
	unsigned int length_;

	Coord start_;
	Coord end_;

	std::queue<CellID> visiting_;

	std::vector<Path> paths_;

	bool simulateStep();
	void printGrid();
	std::string printSymbol(Cell val);
	bool checkCell(int x, int y, Check check);
	bool checkValue(Cell val);
	void calculatePath();
	void clear();
	void clear(std::queue<CellID> &q);
};
