#pragma once

#include <queue>
#include <string>

struct Coord {
	int x;
	int y;
	int z;

	int x_nearest;
	int y_nearest;

	Coord() {
		x = 0;
		y = 0;
		z = 0;
	}
	Coord(unsigned int a, unsigned int b, unsigned int c) {
		x = a;
		y = b;
		z = c;
	}
	std::string print() {
		return std::string("(") + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}
};

struct Path {
	unsigned int id;
	Coord start;
	Coord end;
	Path() {
	}
	Path(Coord s, Coord e) {
		start = s;
		end = e;
	}
	Path(unsigned int xs, unsigned int ys, unsigned int zs, unsigned int xe, unsigned int ye, unsigned int ze) {
		start.x = xs;
		start.y = ys;
		start.z = zs;

		end.x = xe;
		end.y = ye;
		end.z = ze;
	}
	std::string print() {
		return start.print() + " - " + end.print();
	}
};

enum LayerDirection {
	NoZ,
	In,
	Out
};

struct OutputSegment {
	int layer;
	LayerDirection zdir;
	int startx, starty;
	int endx, endy;
};

struct OutputPath {
	std::vector<OutputSegment> paths;
};


enum SimulateStatus {
	STATUS_UNFINISHED = 0,
	STATUS_FAILED,
	STATUS_SUCCESS
};

class GridBase {
public:

    virtual ~GridBase() = 0;

	virtual void addBlockArea(Coord start_coord, Coord bottom_right) = 0;
	virtual void setBlockers(unsigned int size, Coord *coordinates) = 0;
	virtual void addPath(int xs, int ys, int xe, int ye) = 0;
	virtual void addPath(Coord start, Coord end) = 0;
	virtual void addPath(Path path) = 0;
    virtual void simulate() = 0;
};
