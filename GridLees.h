#pragma once

#include <queue>
#include <string>
#include "GridBase.h"

typedef int Cell;

const Cell CELL_PIN		= -1;
const Cell CELL_EMPTY	= -2;
const Cell CELL_BLOCK	= -3;
const Cell CELL_BASE_WIRE_BLOCK = -4;

class GridLees {
public:
	GridLees(unsigned int width, unsigned int length, unsigned int depth);
    ~GridLees();

	void setBlockers(unsigned int size, Coord *coordinates);
	void addPath(Path path);
	//void addtracks(string ori,string start, string step);
    bool route();
private:
    Cell *grid_;

	struct CellID {
		unsigned int x, y, z, dist;
		CellID();
		CellID(unsigned int x, unsigned int y, unsigned int z, unsigned int dist);
	};

	enum Check {
		CHECK_UP = 0,
		CHECK_RIGHT,
		CHECK_DOWN,
		CHECK_LEFT,
	};

    unsigned int width_;
	unsigned int length_;
	unsigned int depth_;
	unsigned int pitch_;
	unsigned int size_;

	Coord start_;
	Coord end_;

	std::queue<CellID> visiting_;
	std::vector<CellID> path_;

	std::vector<Path> paths_;

	bool simulateStep();
	bool routeWire(unsigned int wire_id);

	inline int wireToCell(unsigned int wire);

	void clearRoute(unsigned int wire);
	void rip(unsigned int wire);
	unsigned int heuristicSlope(unsigned int id);

	inline Cell &getGrid(unsigned int x, unsigned int y, unsigned int z);
	
	void printGrid();
	void printPath(unsigned int wire_id);

	std::string printSymbol(Cell val);
	bool checkCell(int x, int y, int z, Check check);
	bool checkValue(Cell val);
	bool calculatePath(unsigned int wire_id);
	void clear();
	void clearQueue(std::queue<CellID> &q);
	void clearTempBlockers();
	void sortPaths();
};
