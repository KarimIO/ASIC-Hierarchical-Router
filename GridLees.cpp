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

GridLees::GridLees(int die_x0, int die_y0, int die_x1, int die_y1, unsigned int d) : depth_(d) {
	x0_ = die_x0;
	y0_ = die_y0;

	width_  = die_x1 - die_x0 + 1;
	length_ = die_y1 - die_y0 + 1;

	std::cout << "Die Width: " << width_ << "\n";
	std::cout << "Die Length: " << length_ << "\n";

	debug_mode_ = 0;

	layers_.reserve(d);
}

void GridLees::setBlockers(unsigned int size, Coord *c) {
	for (int i = 0; i < size; ++i) {
		getLocalPos(c[i]);
		getGrid(c[i].x_nearest, c[i].y_nearest, c[i].z) = CELL_BLOCK;
	}
}

bool GridLees::canGoToLayer(unsigned int from_x, unsigned int from_y, unsigned int from_z, unsigned int to_z, unsigned int &to_x, unsigned int &to_y) {
	Coord origin(from_x, from_y, from_z);
	getGlobalPos(origin);

	Layer &l = layers_[to_z];

	if (l.is_horizontal) {
		int rem = (origin.y - l.start_coord) % l.stride;
		if (rem == 0) {
			int y = (origin.y - l.start_coord) / l.stride;

			if (y >= l.num_steps)
				return false;

			to_x = origin.x - x0_;
			to_y = y;

			return true;
		}
	}
	else {
		int rem = (origin.x - l.start_coord) % l.stride;
		if (rem == 0) {
			int x = (origin.x - l.start_coord) / l.stride;

			if (x >= l.num_steps)
				return false;

			to_x = x;
			to_y = origin.y - y0_;

			return true;
		}
	}

	return false;
}

void GridLees::getGlobalPos(Coord &c) {
	Layer &l = layers_[c.z];

	if (l.is_horizontal) {
		int y = c.y * l.stride + l.start_coord;

		c.x = c.x + x0_;
		c.y = y;
	}
	else {
		int x = c.x * l.stride + l.start_coord;

		c.x = x;
		c.y = c.y + y0_;
	}
}

void GridLees::setDebugMode(bool mid_path, bool mid_grid, bool show_add_pins, bool show_add_tracks) {
	debug_mode_ = (mid_path * DEBUG_MID_PATHS) | (mid_grid * DEBUG_MID_GRID) | (show_add_pins * DEBUG_ADD_PINS) | (show_add_tracks * DEBUG_ADD_TRACKS);
}

void GridLees::getLocalPos(Coord &c) {
	Layer &l = layers_[c.z];

	if (l.is_horizontal) {
		double y = double(c.y - l.start_coord) / l.stride;
		y = ((y >= l.num_steps) ? l.num_steps - 1 : y);
		y = ((y < 0) ? 0 : y);

		c.x_nearest = c.x - x0_;
		c.y_nearest = round(y);
	}
	else {
		double x = double(c.x - l.start_coord) / l.stride;
		x = ((x >= l.num_steps) ? l.num_steps - 1 : x);
		x = ((x < 0) ? 0 : x);

		c.x_nearest = round(x);
		c.y_nearest = c.y - y0_;
	}
}

void GridLees::addPath(Path p) {
	paths_.emplace_back(p);
	Path &v = paths_.back();
	v.id = paths_.size() - 1;
	getLocalPos(v.start);
	getLocalPos(v.end);

	if (debug_mode_ & DEBUG_ADD_PINS) {
		std::cout << "Pin Start " << paths_.size() << ": " << v.start.print() << " -> (" << v.start.x_nearest << ", " << v.start.y_nearest << ")\n";
		std::cout << "Pin End " << paths_.size() << ": " << v.end.print() << " -> (" << v.end.x_nearest << ", " << v.end.y_nearest << ")\n";
	}

	// Add Blockers under pins
	for (int i = 0; i < v.start.z; ++i) {
		Coord e = v.start;
		e.z = i;
		getLocalPos(e);
		getGrid(e.x_nearest, e.y_nearest, e.z) = CELL_BLOCK;
	}

	for (int i = 0; i < v.end.z; ++i) {
		Coord e = v.end;
		e.z = i;
		getLocalPos(e);
		getGrid(e.x_nearest, e.y_nearest, e.z) = CELL_BLOCK;
	}

	getGrid(v.start.x_nearest, v.start.y_nearest, v.start.z) = CELL_PIN;
	getGrid(v.end.x_nearest, v.end.y_nearest, v.end.z) = CELL_PIN;
}

bool GridLees::checkValue(Cell val) {
    return (val == CELL_EMPTY);
}

void GridLees::addLayer(int start_coord, unsigned int num_steps, unsigned int stride, bool is_horizontal) {
	layers_.push_back(Layer());
	Layer &layer = layers_.back();
	layer.is_horizontal = is_horizontal;
	layer.start_coord = start_coord;
	layer.num_steps = num_steps;
	layer.stride = stride;

	if (is_horizontal) {
		layer.width = width_;
		layer.length = num_steps;
	}
	else {
		layer.width = num_steps;
		layer.length = length_;
	}

	layer.size = layer.width * layer.length;
	layer.grid = new Cell[layer.size];

	if (debug_mode_ | DEBUG_ADD_TRACKS) {
		std::cout << "Adding " << (layer.is_horizontal?"X":"Y") << " Track:";
		std::cout << "\n\tStart Coordinate: " << layer.start_coord;
		std::cout << "\n\tNumber of Steps: " << layer.num_steps;
		std::cout << "\n\tStride: " << layer.stride << "\n";
	}

	for (int i = 0; i < layer.size; ++i) {
		layer.grid[i] = CELL_EMPTY;
	}
}

SimulateStatus GridLees::simulateStep() {
	if (visiting_.size() == 0) {
		return STATUS_FAILED;
	}
	
	CellID id;
    id = visiting_.front();
    visiting_.pop();

    if (id.x == end_.x_nearest && id.y == end_.y_nearest && end_.z == id.z) {
        return STATUS_SUCCESS;
    }
    else {
        int v = getGrid(id.x, id.y, id.z) + 1;

		if (layers_[id.z].is_horizontal) {
			if (id.x > 0) {
				if (checkValue(getGrid(id.x - 1, id.y, id.z))) {
					visiting_.emplace(id.x - 1, id.y, id.z, v);
					getGrid(id.x - 1, id.y, id.z) = v;
				}
				else if (end_.z == id.z && end_.y_nearest == id.y && end_.x_nearest == id.x - 1) {
					return STATUS_SUCCESS;
				}
			}

			if (id.x < width_ - 1) {
				if (checkValue(getGrid(id.x + 1, id.y, id.z))) {
					visiting_.emplace(id.x + 1, id.y, id.z, v);
					getGrid(id.x + 1, id.y, id.z) = v;
				}
				else if (end_.z == id.z && end_.y_nearest == id.y && end_.x_nearest == id.x + 1) {
					return STATUS_SUCCESS;
				}
			}
		}
		else {
			if (id.y > 0) {
				if (checkValue(getGrid(id.x, id.y - 1, id.z))) {
					visiting_.emplace(id.x, id.y - 1, id.z, v);
					getGrid(id.x, id.y - 1, id.z) = v;
				}
				else if (end_.z == id.z && end_.y_nearest == id.y - 1 && end_.x_nearest == id.x) {
					return STATUS_SUCCESS;
				}
			}

			if (id.y < length_ - 1) {
				if (checkValue(getGrid(id.x, id.y + 1, id.z))) {
					visiting_.emplace(id.x, id.y + 1, id.z, v);
					getGrid(id.x, id.y + 1, id.z) = v;
				}
				else if (end_.z == id.z && end_.y_nearest == id.y + 1 && end_.x_nearest == id.x) {
					return STATUS_SUCCESS;
				}
			}
		}

		if (id.z > 0) {
			unsigned int x;
			unsigned int y;
			if (canGoToLayer(id.x, id.y, id.z, id.z - 1, x, y)) {
				if (checkValue(getGrid(x, y, id.z - 1))) {
					visiting_.emplace(x, y, id.z - 1, v);
					getGrid(x, y, id.z - 1) = v;
				}
				else if (end_.z == id.z - 1 && end_.y_nearest == y && end_.x_nearest == x) {
					return STATUS_SUCCESS;
				}
			}
		}

		if (id.z < depth_ - 1) {
			unsigned int x;
			unsigned int y;
			if (canGoToLayer(id.x, id.y, id.z, id.z + 1, x, y)) {
				if (checkValue(getGrid(x, y, id.z + 1))) {
					visiting_.emplace(x, y, id.z + 1, v);
					getGrid(x, y, id.z + 1) = v;
				}
				else if (end_.z == id.z + 1 && end_.y_nearest == y && end_.x_nearest == x) {
					return STATUS_SUCCESS;
				}
			}
		}
    }

    return STATUS_UNFINISHED;
}

bool GridLees::calculatePath(unsigned int wire_id) {
	bool finished = false;

    int x = end_.x_nearest;
	int y = end_.y_nearest;
	int z = end_.z;
	
    int min = INT_MAX;
    while (true) {
        int xmin = x;
		int ymin = y;
		int zmin = z;

		if (layers_[z].is_horizontal) {
			if (x > 0 && getGrid(x - 1, y, z) >= 0 && getGrid(x - 1, y, z) < min) {
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
		}
		else {
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
		}

		if (z > 0) {
			unsigned int new_x;
			unsigned int new_y;
			if (canGoToLayer(x, y, z, z - 1, new_x, new_y)) {
				if (getGrid(new_x, new_y, z - 1) >= 0 && getGrid(new_x, new_y, z - 1) < min) {
					min = getGrid(new_x, new_y, z - 1);
					xmin = new_x;
					ymin = new_y;
					zmin = z - 1;
				}
			}
		}

		if (z < depth_ - 1) {
			unsigned int new_x;
			unsigned int new_y;
			if (canGoToLayer(x, y, z, z + 1, new_x, new_y)) {
				if (getGrid(new_x, new_y, z + 1) >= 0 && getGrid(new_x, new_y, z + 1) < min) {
					min = getGrid(new_x, new_y, z + 1);
					xmin = new_x;
					ymin = new_y;
					zmin = z + 1;
				}
			}
		}

        if (min == 0) {
			finished = true;
            break;
        }

		getGrid(xmin, ymin, zmin) = wireToCell(wire_id);

		if (xmin == x && ymin == y && zmin == z) {
			return false;
		}

        x = xmin;
		y = ymin;
		z = zmin;
    }

    return finished;
}


void GridLees::printPath(unsigned int wire_id) {
    wire_id = wireToCell(wire_id);

    for (int i = 0; i < depth_; ++i) {
		Layer &l = layers_[i];
		for (int j = 0; j < l.length; ++j) {
			for (int k = 0; k < l.width; ++k) {
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
	return layers_[z].grid[y * layers_[z].width + x];
}

bool GridLees::routeWire(unsigned int wire_id) {
    Path &wire = paths_[wire_id];
    start_ = wire.start;
    end_ = wire.end;
	getGrid(start_.x_nearest, start_.y_nearest, start_.z) = 0;

    visiting_.emplace(start_.x_nearest, start_.y_nearest, start_.z, 0);

	SimulateStatus status;

    // Wait for timeout, in case of bugs
    while ((status = simulateStep()) == STATUS_UNFINISHED);

    bool empty = (visiting_.size() == 0);

    clearQueue(visiting_);

	if (debug_mode_ & DEBUG_MID_GRID)
		printGrid();

    if (status == STATUS_FAILED) {
        std::cout << "Could not find path to end!\n";
		getGrid(start_.x_nearest, start_.y_nearest, start_.z) = CELL_PIN;

        clear();

        return false;
    }
    else {        
        if (!calculatePath(wire_id)) {
			getGrid(start_.x_nearest, start_.y_nearest, start_.z) = CELL_PIN;
			if (debug_mode_ & DEBUG_MID_PATHS)
					printPath(wire_id);
            return false;
        }

		if (debug_mode_ & DEBUG_MID_PATHS)
			printPath(wire_id);
        
    }

	getGrid(start_.x_nearest, start_.y_nearest, start_.z) = CELL_PIN;

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
	for (int i = 0; i < depth_; ++i) {
		Layer &l = layers_[i];
		for (int j = 0; j < l.size; ++j) {
			if (l.grid[j] > 0) {
				l.grid[j] = CELL_EMPTY;
			}
		}
	}
}

inline int GridLees::wireToCell(unsigned int wire) {
    return CELL_BASE_WIRE_BLOCK - wire;
}

void GridLees::clearRoute(unsigned int wire) {
    int wire_id = wireToCell(wire);

	for (int i = 0; i < depth_; ++i) {
		Layer &l = layers_[i];
		for (int j = 0; j < l.size; ++j) {
			if (l.grid[j] == wire_id) {
				l.grid[j] = CELL_EMPTY;
			}
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
		Layer &l = layers_[i];
		for (int j = 0; j < l.length; ++j) {
			for (int k = 0; k < l.width; ++k) {
				std::cout << std::setw(2) << printSymbol(getGrid(k, j, i)) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "--------------------\n";
	}
	std::cout << "====================\n";
}

void GridLees::addBlockArea(Coord min_coord, Coord max_coord) {

	getLocalPos(min_coord);
	getLocalPos(max_coord);

	for (int z = min_coord.z; z <= max_coord.z; ++z) {
		for (int y = min_coord.y_nearest; y <= max_coord.y_nearest; ++y) {
			for (int x = min_coord.x_nearest; x <= max_coord.x_nearest; ++x) {
				getGrid(x, y, z) = CELL_BLOCK;
			}
		}
	}
}

void GridLees::printPaths() {
	std::string val;
	for (int i = 0; i < depth_; ++i) {
		Layer &l = layers_[i];
		for (int j = 0; j < l.length; ++j) {
			for (int k = 0; k < l.width; ++k) {
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
					fix_val = paths_[fix_val].id;
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
	for (int i = 0; i < depth_; ++i) {
		if (layers_[i].grid != nullptr)
			delete[] layers_[i].grid;
	}
}