#pragma once

#include <queue>
#include <string>
#include "GridBase.h"
#include <map>
#include <vector>

typedef int Cell;

const Cell CELL_PIN		= -1;
const Cell CELL_EMPTY	= -2;
const Cell CELL_BLOCK	= -3;
const Cell CELL_BASE_WIRE_BLOCK = -4;

const unsigned int DEBUG_MID_PATHS		= 0x1;
const unsigned int DEBUG_MID_GRID		= 0x2;
const unsigned int DEBUG_ADD_PINS		= 0x4;
const unsigned int DEBUG_ADD_TRACKS		= 0x8;

class GridLees {
public:
	GridLees(int die_x0, int die_y0, int die_x1, int die_y1, unsigned int depth);
    ~GridLees();

	void setBlockers(unsigned int size, Coord *coordinates);
	void addPath(Path path);
    bool route();
	void printPaths();
	std::vector<OutputPath> getPaths();

	void addBlockArea(Coord min_coord, Coord max_coord);
	void addLayer(int start_coord, unsigned int num_steps, unsigned int stride, bool is_horizontal);
	void setDebugMode(bool mid_path, bool mid_grid, bool show_add_pins, bool show_add_tracks);
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

	unsigned int debug_mode_;

	Coord start_;
	Coord end_;

	std::queue<CellID> visiting_;

	std::vector<Path> paths_;
	//std::vector<std::string,std::pair<int,int>> outputpaths;

	bool canGoToLayer(unsigned int from_x, unsigned int from_y, unsigned int from_z, unsigned int to_z, unsigned int &to_x, unsigned int &to_y);

	void getGlobalPos(Coord &c);
	void getLocalPos(Coord &c);

	SimulateStatus simulateStep();
	bool routeWire(unsigned int wire_id);

	int wireToCell(unsigned int wire);

	void clearRoute(unsigned int wire);
	void rip(unsigned int wire);

	void printArea(Coord start_p, Coord end_p);

	unsigned int heuristicClose(unsigned int id);
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
