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
	GridLees(int die_x0, int die_y0, int die_x1, int die_y1, unsigned int depth);
    ~GridLees();

	void setBlockers(unsigned int size, Coord *coordinates);
	void addPath(Path path);
    bool route();
	void printPaths();

	void addLayer(int start_coord, unsigned int num_steps, unsigned int stride, bool is_horizontal);
private:
	struct Layer {
		bool is_horizontal;
		int start_coord;
		unsigned int num_steps;
		unsigned int stride;
		unsigned int width;
		unsigned int length;
		unsigned int size;
		Cell *grid;
	};

	std::vector<Layer> layers_;

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
		CHECK_IN,
		CHECK_OUT
	};

	int x0_;
	int y0_;
	unsigned int width_;
	unsigned int length_;
	unsigned int depth_;
	unsigned int pitch_;
	unsigned int size_;

	Coord start_;
	Coord end_;

	std::queue<CellID> visiting_;

	std::vector<Path> paths_;

	bool canGoToLayer(unsigned int from_x, unsigned int from_y, unsigned int from_z, unsigned int to_z, unsigned int &to_x, unsigned int &to_y);

	void getGlobalPos(Coord &c);
	void getLocalPos(Coord &c);

	SimulateStatus simulateStep();
	bool routeWire(unsigned int wire_id);

	int wireToCell(unsigned int wire);

	void clearRoute(unsigned int wire);
	void rip(unsigned int wire);
	unsigned int heuristicSlope(unsigned int id);

	Cell &getGrid(unsigned int x, unsigned int y, unsigned int z);
	
	void printGrid();
	void printPath(unsigned int wire_id);

	std::string printSymbol(Cell val);
	bool checkValue(Cell val);
	bool calculatePath(unsigned int wire_id);
	void clear();
	void clearQueue(std::queue<CellID> &q);
	void sortPaths();
};
