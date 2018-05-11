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
	std::string print() {
		return std::string("(") + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};

struct Path {
	unsigned int priority;

	Coord start;
	Coord end;
	Path() {
		priority = 0;
	}
	Path(Coord s, Coord e) {
		start = s;
		end = e;
		priority = 0;
	}
	Path(int xs, int ys, int xe, int ye) {
		start.x = xs;
		start.y = ys;

		end.x = xe;
		end.y = ye;

		priority = 0;
	}
	std::string print() {
		return start.print() + " - " + end.print();
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
