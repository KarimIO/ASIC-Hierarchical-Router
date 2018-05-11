#include "GridLees.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>

const bool DEBUG_PATH = false;
double path_size;
unsigned int max_priority_;

GridLees::CellID::CellID(int x, int y, int dist) {
	this->x = x;
	this->y = y;
	this->dist = dist;
}

GridLees::GridLees(unsigned int w, unsigned int l) : width_(w), length_(l) {
    if (w < 0 || l < 0) {
        throw std::runtime_error("Invalid Size for Grid.");
    }

	max_priority_ = 0;

    grid_ = new Cell[l * w];

    for (int i = 0; i < l * w; ++i) {
		grid_[i] = CELL_EMPTY;
    }
}

void GridLees::setBlockers(unsigned int size, Coord *c) {
	for (int i = 0; i < size; ++i) {
		grid_[c[i].y * width_ + c[i].x] = CELL_BLOCK;
	}
}
void GridLees::addPath(int xs, int ys, int xe, int ye) {
	paths_.emplace_back(Path(xs, ys, xe, ye));
	path_size = paths_.size();

	grid_[ys * width_ + xs] = CELL_PIN;
	grid_[ye * width_ + xe] = CELL_PIN;

}

void GridLees::addPath(Coord start, Coord end) {
	paths_.emplace_back(Path(start, end));

	grid_[start.y * width_ + start.x] = CELL_PIN;
	grid_[end.y * width_ + end.x] = CELL_PIN;
}

void GridLees::addPath(Path p) {
	paths_.emplace_back(p);

	grid_[p.start.y * width_ + p.start.x] = CELL_PIN;
	grid_[p.end.y * width_ + p.end.x] = CELL_PIN;
}

bool GridLees::checkValue(Cell val) {
    return (val == CELL_EMPTY);
}

bool GridLees::checkCell(int x, int y, Check check) {
    switch (check) {
        case CHECK_UP:
            if (y < length_ - 1)
                return checkValue(grid_[(y + 1) * width_ + x]);
        case CHECK_DOWN:
            if (y > 0)
                return checkValue(grid_[(y - 1) * width_ + x]);
        case CHECK_RIGHT:
            if (x < width_ - 1)
                return checkValue(grid_[y * width_ + x + 1]);
        case CHECK_LEFT:
            if (x > 0)
                return checkValue(grid_[y * width_ + x - 1]);
    }

    return false;
}

bool GridLees::simulateStep() {
    CellID id;
    id = visiting_.front();
    visiting_.pop();

    if (id.x == end_.x && id.y == end_.y) {
        return false;
    }
    else {
        int v = grid_[id.y * width_ + id.x] + 1;

        if (id.x > 0) {
            if (checkValue(grid_[id.y * width_ + id.x - 1])) {
                visiting_.emplace(id.x - 1, id.y, v);
                grid_[id.y * width_ + id.x - 1] = v;
            }
            else if (end_.y == id.y && end_.x == id.x - 1) {
                return false;
            }
        }

        if (id.x < width_ - 1) {
            if (checkValue(grid_[id.y * width_ + id.x + 1])) {
                visiting_.emplace(id.x + 1, id.y, v);
                grid_[id.y * width_ + id.x + 1] = v;
            }
            else if (end_.y == id.y && end_.x == id.x + 1) {
                return false;
            }
        }

        if (id.y > 0) {
            if (checkValue(grid_[(id.y - 1) * width_ + id.x])) {
                visiting_.emplace(id.x, id.y - 1, v);
                grid_[(id.y - 1) * width_ + id.x] = v;
            }
			else if (end_.y == id.y - 1 && end_.x == id.x) {
                return false;
            }
        }

        if (id.y < length_ - 1) {
            if (checkValue(grid_[(id.y + 1) * width_ + id.x])) {
                visiting_.emplace(id.x, id.y + 1, v);
                grid_[(id.y + 1) * width_ + id.x] = v;
            }
			else if (end_.y == id.y + 1 && end_.x == id.x) {
                return false;
            }
        }
    }

    return true;
}

bool GridLees::calculatePath(unsigned int wire_id) {
	bool finished = false;

    int x = end_.x;
    int y = end_.y;
    int min = INT32_MAX;
    while (true) {
        int xmin = x;
        int ymin = y;

        if (x > 0 && grid_[y * width_ + x - 1] >= 0 && grid_[y * width_ + x - 1] < min) {
            path_.emplace_back(x - 1, y, 0);
            min = grid_[y * width_ + x - 1];
            xmin = x - 1;
            ymin = y;
        }

        if (x < width_ - 1 && grid_[y * width_ + x + 1] >= 0 && grid_[y * width_ + x + 1] < min) {
            min = grid_[y * width_ + x + 1];
            xmin = x + 1;
            ymin = y;
        }

        if (y > 0 && grid_[(y - 1) * width_ + x] >= 0 && grid_[(y - 1) * width_ + x] < min) {
            min = grid_[(y - 1) * width_ + x];
            xmin = x;
            ymin = y - 1;
        }

        if (y < length_ - 1 && grid_[(y + 1) * width_ + x] >= 0 && grid_[(y + 1) * width_ + x] < min) {
            min = grid_[(y + 1) * width_ + x];
            xmin = x;
            ymin = y + 1;
        }
        
        if (min == 0) {
			finished = true;
            break;
        }

		grid_[ymin * width_ + xmin] = wireToCell(wire_id);
        path_.emplace_back(xmin, ymin, 0);

        x = xmin;
        y = ymin;
    }

    return finished;
}


void GridLees::printPath(unsigned int wire_id) {
    wire_id = wireToCell(wire_id);

    for (int i = 0; i < length_; ++i) {
        for (int j = 0; j < width_; ++j) {
			Cell val = grid_[i * width_ + j];

            if (val == wire_id) {
                std::cout << "x ";
            } 
            else if (val == CELL_PIN) {
                std::cout << "p ";
            }
            else if (val == CELL_BLOCK || val <= CELL_BASE_WIRE_BLOCK) {
                std::cout << "# ";
            }
            else {
                std::cout << "_ ";
            }
        }

        std::cout << "\n";
    }
}

void GridLees::clearTempBlockers() {
	for (int i = 0; i < length_ * width_; ++i) {
		if (grid_[i] <= CELL_BASE_WIRE_BLOCK)
			grid_[i] = CELL_EMPTY;
	}
}

void GridLees::clearQueue(std::queue<CellID> &q) {
	std::queue<CellID> empty;
	std::swap(q, empty);
}

double length(Path &i) {
	double x = i.end.x - i.start.x;
	double y = i.end.y - i.start.y;
	return sqrt(x * x + y * y);
}

double getPathValue(Path &i) {
	unsigned int priority = max_priority_ - i.priority;
	double val = (priority * path_size);

	return val + length(i);
}

bool compare(Path &i, Path &j) { return getPathValue(i) < getPathValue(j); }

double slope(Path &p) {
    return (p.start.y - p.end.y) / (p.start.x - p.end.x);
}

unsigned int GridLees::heuristicSlope(unsigned int id) {
    double a = slope(paths_[id]);
    double b = 0;

    unsigned int min_slope_id = 0;
    
    for (int i = 1; i < paths_.size(); ++i) {
        double b = slope(paths_[i]);
    }

    return min_slope_id;
}

void GridLees::sortPaths() {
	std::sort(paths_.begin(), paths_.end(), compare);

	/*for (auto &p : paths_) {
		std::cout << "Path: (" << getPathValue(p) << ") " << p.print() << "\n";
	}*/
}

inline Cell &GridLees::getGrid(unsigned int x, unsigned int y) {
    return grid_[y * width_ + x];
}

bool GridLees::routeWire(unsigned int wire_id) {
    Path &wire = paths_[wire_id];
    start_ = wire.start;
    end_ = wire.end;
    grid_[start_.y * width_ + start_.x] = 0;

    visiting_.emplace(start_.x, start_.y, 0);

    // Wait for timeout, in case of bugs
    while (visiting_.size() > 0 && simulateStep());

    bool empty = (visiting_.size() == 0);

    clearQueue(visiting_);

    if (empty) {
        std::cout << "Could not find path to end!\n";
	    grid_[start_.y * width_ + start_.x] = CELL_PIN;

        clear();

        return false;
    }
    else {        
        if (!calculatePath(wire_id)) {
	        grid_[start_.y * width_ + start_.x] = CELL_PIN;
            return false;
        }
        
    }

	grid_[start_.y * width_ + start_.x] = CELL_PIN;

    clearQueue(visiting_);
    clear();

    return true;
}

void GridLees::rip(unsigned int wire) {
    Path p = paths_[wire];
    paths_.erase(paths_.begin() + wire);
    paths_.push_back(p);
}

bool GridLees::route() {
	bool finished = false;
	bool status;

    // Sort routes by uler distance
	sortPaths();

    // Try Routing
    for (int i = 0; i < paths_.size(); ++i) {
        // Try to route the wire
        status = routeWire(i);

        // If not routed it successfully
        if (!status) {
            if (i < 1) {
                return false;
            }

            unsigned int id = heuristicSlope(i);
            rip(id);
            std::cout << "Ripping path: " << paths_[id].print() << "\n";

            // Clear Pin Paths
            clearRoute(i);
            clearRoute(id);

            // Redo the current route. (-1 to repeat, another -1 because we ripped a node.)
            i -= 2;
        }
	}

    return true;
}

void GridLees::clear() {
	for (int i = 0; i < width_ * length_; ++i) {
		if (grid_[i] > 0) {
			grid_[i] = CELL_EMPTY;
		}
	}
}

inline int GridLees::wireToCell(unsigned int wire) {
    return CELL_BASE_WIRE_BLOCK - wire;
}

void GridLees::clearRoute(unsigned int wire) {
    int wire_id = wireToCell(wire);

	for (int i = 0; i < width_ * length_; ++i) {
		if (grid_[i] == wire_id) {
			grid_[i] = CELL_EMPTY;
		}
	}
}

std::string GridLees::printSymbol(Cell val) {
    if (val > 0)
        return std::to_string(val);
    else if (val == CELL_EMPTY)
        return "__";
    else if (val == 0 || val == CELL_PIN)
		return "00";
    else
        return "##";
}

void GridLees::printGrid() {
    for (int i = 0; i < length_; ++i) {
        for (int j = 0; j < width_; ++j) {
            std::cout << std::setw(2) << printSymbol(grid_[i * width_ + j]) << " ";
        }
        std::cout << "\n";
    }
}

GridLees::~GridLees() {
    if (grid_ != nullptr)
        delete[] grid_;
}