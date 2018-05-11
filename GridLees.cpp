#include "GridLees.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>

const bool DEBUG_PATH = false;
double path_size;

GridLees::CellID::CellID() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->dist = 0;
}

GridLees::CellID::CellID(unsigned int x, unsigned int y, unsigned int z, unsigned int dist) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->dist = dist;
}

GridLees::GridLees(unsigned int w, unsigned int l, unsigned int d) : width_(w), length_(l), depth_(d), pitch_(l * w), size_(pitch_ * d) {
    if (w < 0 || l < 0 || d < 0) {
        throw std::runtime_error("Invalid Size for Grid.");
    }

    grid_ = new Cell[size_];

    for (int i = 0; i < size_; ++i) {
		grid_[i] = CELL_EMPTY;
    }
}

void GridLees::setBlockers(unsigned int size, Coord *c) {
	for (int i = 0; i < size; ++i) {
		getGrid(c[i].x, c[i].y, c[i].z) = CELL_BLOCK;
	}
}

void GridLees::addPath(Path p) {
	paths_.emplace_back(p);

	getGrid(p.start.x, p.start.y, p.start.z) = CELL_PIN;
	getGrid(p.end.x, p.end.y, p.end.z) = CELL_PIN;
}

bool GridLees::checkValue(Cell val) {
    return (val == CELL_EMPTY);
}

bool GridLees::checkCell(int x, int y, int z, Check check) {
    switch (check) {
        case CHECK_UP:
            if (y < length_ - 1)
                return checkValue(getGrid(x, y + 1, z));
        case CHECK_DOWN:
            if (y > 0)
                return checkValue(getGrid(x, y - 1, z));
		case CHECK_RIGHT:
			if (x < width_ - 1)
				return checkValue(getGrid(x + 1, y, z));
		case CHECK_LEFT:
			if (x > 0)
				return checkValue(getGrid(x - 1, y, z));
		case CHECK_OUT:
			if (z < depth_ - 1)
				return checkValue(getGrid(x, y, z + 1));
		case CHECK_IN:
			if (z > 0)
				return checkValue(getGrid(x, y, z - 1));
    }

    return false;
}

bool GridLees::simulateStep() {
    CellID id;
    id = visiting_.front();
    visiting_.pop();

    if (id.x == end_.x && id.y == end_.y && end_.z == id.z) {
        return false;
    }
    else {
        int v = getGrid(id.x, id.y, id.z) + 1;

        if (id.x > 0) {
            if (checkValue(getGrid(id.x - 1, id.y, id.z))) {
                visiting_.emplace(id.x - 1, id.y, id.z, v);
                getGrid(id.x - 1, id.y, id.z) = v;
            }
            else if (end_.z == id.z && end_.y == id.y && end_.x == id.x - 1) {
                return false;
            }
        }

        if (id.x < width_ - 1) {
            if (checkValue(getGrid(id.x + 1, id.y, id.z))) {
                visiting_.emplace(id.x + 1, id.y, id.z, v);
				getGrid(id.x + 1, id.y, id.z) = v;
            }
            else if (end_.z == id.z && end_.y == id.y && end_.x == id.x + 1) {
                return false;
            }
        }

		if (id.y > 0) {
			if (checkValue(getGrid(id.x, id.y - 1, id.z))) {
				visiting_.emplace(id.x, id.y - 1, id.z, v);
				getGrid(id.x, id.y - 1, id.z) = v;
			}
			else if (end_.z == id.z && end_.y == id.y - 1 && end_.x == id.x) {
				return false;
			}
		}

		if (id.y < length_ - 1) {
			if (checkValue(getGrid(id.x, id.y + 1, id.z))) {
				visiting_.emplace(id.x, id.y + 1, id.z, v);
				getGrid(id.x, id.y + 1, id.z) = v;
			}
			else if (end_.z == id.z && end_.y == id.y + 1 && end_.x == id.x) {
				return false;
			}
		}

		if (id.z > 0) {
			if (checkValue(getGrid(id.x, id.y, id.z - 1))) {
				visiting_.emplace(id.x, id.y, id.z - 1, v);
				getGrid(id.x, id.y, id.z - 1) = v;
			}
			else if (end_.z == id.z - 1 && end_.y == id.y && end_.x == id.x) {
				return false;
			}
		}

		if (id.z < depth_ - 1) {
			if (checkValue(getGrid(id.x, id.y, id.z + 1))) {
				visiting_.emplace(id.x, id.y, id.z + 1, v);
				getGrid(id.x, id.y, id.z + 1) = v;
			}
			else if (end_.z == id.z + 1 && end_.y == id.y && end_.x == id.x) {
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
	int z = end_.z;
    int min = INT32_MAX;
    while (true) {
        int xmin = x;
		int ymin = y;
		int zmin = z;

        if (x > 0 && getGrid(x - 1, y, z) >= 0 && getGrid(x - 1, y, z) < min) {
            //path_.emplace_back(x - 1, y, z);
            min = getGrid(x - 1, y, z);
            xmin = x - 1;
			ymin = y;
			zmin = z;
        }

        if (x < width_ - 1 && getGrid(x + 1, y, z) >= 0 && getGrid(x + 1, y, z) < min) {
            min = getGrid(x + 1, y, z);
            xmin = x + 1;
            ymin = y;
			zmin = z;
        }

		if (y > 0 && getGrid(x, y - 1, z) >= 0 && getGrid(x, y - 1, z) < min) {
			min = getGrid(x, y - 1, z);
			xmin = x;
			ymin = y - 1;
			zmin = z;
		}

		if (y < length_ - 1 && getGrid(x, y + 1, z) >= 0 && getGrid(x, y + 1, z) < min) {
			min = getGrid(x, y + 1, z);
			xmin = x;
			ymin = y + 1;
			zmin = z;
		}

		if (z > 0 && getGrid(x, y, z - 1) >= 0 && getGrid(x, y, z - 1) < min) {
			min = getGrid(x, y, z - 1);
			xmin = x;
			ymin = y;
			zmin = z - 1;
		}

		if (z < depth_ - 1 && getGrid(x, y, z + 1) >= 0 && getGrid(x, y, z + 1) < min) {
			min = getGrid(x, y, z + 1);
			xmin = x;
			ymin = y;
			zmin = z + 1;
		}
        
        if (min == 0) {
			finished = true;
            break;
        }

		getGrid(xmin, ymin, zmin) = wireToCell(wire_id);
        path_.emplace_back(xmin, ymin, zmin, min);

        x = xmin;
		y = ymin;
		z = zmin;
    }

    return finished;
}


void GridLees::printPath(unsigned int wire_id) {
    wire_id = wireToCell(wire_id);

    for (int i = 0; i < depth_; ++i) {
		for (int j = 0; j < length_; ++j) {
			for (int k = 0; k < width_; ++k) {
				Cell val = getGrid(k, j, i);

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
	return length(i);
}

bool compare(Path &i, Path &j) { return getPathValue(i) < getPathValue(j); }

double slope(Path &p) {
    double a = (p.start.y - p.end.y);
    double b = (p.start.x - p.end.x);
    return a / b;
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

inline Cell &GridLees::getGrid(unsigned int x, unsigned int y, unsigned int z) {
    return grid_[z * pitch_ + y * width_ + x];
}

bool GridLees::routeWire(unsigned int wire_id) {
    Path &wire = paths_[wire_id];
    start_ = wire.start;
    end_ = wire.end;
	getGrid(start_.x, start_.y, start_.z) = 0;

    visiting_.emplace(start_.x, start_.y, start_.z, 0);

    // Wait for timeout, in case of bugs
    while (visiting_.size() > 0 && simulateStep());

    bool empty = (visiting_.size() == 0);

    clearQueue(visiting_);

    if (empty) {
        std::cout << "Could not find path to end!\n";
		getGrid(start_.x, start_.y, start_.z) = CELL_PIN;

        clear();

        return false;
    }
    else {        
        if (!calculatePath(wire_id)) {
			getGrid(start_.x, start_.y, start_.z) = CELL_PIN;
            return false;
        }
        
    }

	getGrid(start_.x, start_.y, start_.z) = CELL_PIN;

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
        std::cout << "Attempting to route: (" << 100.0f * float(i)/float(paths_.size()) << "%) " << paths_[i].print() << "\n";
        // Try to route the wire
        status = routeWire(i);

        // If not routed it successfully
        if (!status) {
            if (i < 1) {
                return false;
            }

            unsigned int id = heuristicSlope(i);
            rip(id);
            std::cout << "Failed; Ripping path: " << paths_[id].print() << "\n";

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
	for (int i = 0; i < size_; ++i) {
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

	for (int i = 0; i < size_; ++i) {
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
	for (int i = 0; i < depth_; ++i) {
		for (int j = 0; j < length_; ++j) {
			for (int k = 0; k < width_; ++k) {
				std::cout << std::setw(2) << printSymbol(getGrid(k, j, i)) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "--------------------\n";
	}
	std::cout << "====================\n";
}

void GridLees::printPaths() {
	std::string val;
	for (int i = 0; i < depth_; ++i) {
		for (int j = 0; j < length_; ++j) {
			for (int k = 0; k < width_; ++k) {
				Cell v = getGrid(k, j, i);
				if (v == CELL_EMPTY) {
					val = "_";
				}
				else if (v == CELL_BLOCK) {
					val = "#";
				}
				else if (v == CELL_PIN) {
					val = "p";
				}
				else if (v <= CELL_BASE_WIRE_BLOCK) {
					int fix_val = CELL_BASE_WIRE_BLOCK - v;
					val = std::to_string(fix_val);
				}
				else {
					val = "?";
				}
				std::cout << std::setw(3) << val << " ";
			}
			std::cout << "\n";
		}
		std::cout << "--------------------\n";
	}
	std::cout << "====================\n";
}

GridLees::~GridLees() {
    if (grid_ != nullptr)
        delete[] grid_;
}