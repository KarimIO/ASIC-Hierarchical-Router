#pragma once

#include <queue>
#include <string>

struct Coord {
	int x;
	int y;
	Coord() {
		x = 0;
		y = 0;
	}
	Coord(int a, int b) {
		x = a;
		y = b;
	}
};

struct Path {
	Coord start;
	Coord end;
	Path() {}
	Path(Coord s, Coord e) {
		start = s;
		end = e;
	}
	Path(int xs, int ys, int xe, int ye) {
		start.x = xs;
		start.y = ys;

		end.x = xe;
		end.y = ye;
	}
};

class GridBase {
public:
    virtual ~GridBase() = 0;

	virtual void setBlockers(unsigned int size, Coord *coordinates) = 0;
	virtual void addPath(int xs, int ys, int xe, int ye) = 0;
	virtual void addPath(Coord start, Coord end) = 0;
	virtual void addPath(Path path) = 0;
    virtual void simulate() = 0;
};
