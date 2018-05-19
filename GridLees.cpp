#include "GridLees.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <climits>
#include <chrono>

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

	getGrid(x, y, z) = wireToCell(wire_id);
	
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

		getGrid(xmin, ymin, zmin) = wireToCell(wire_id);

        if (min == 0) {
			finished = true;
            break;
        }

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
	std::cout<<wire_id<<std::endl;
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

double length(int x1, int y1, int x2, int y2) {
	double x = x2 - x1;
	double y = y2 - y1;
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
	double min_slope_diff = 0;

	for (int i = 1; i < paths_.size(); ++i) {
		double b = slope(paths_[i]);
		double diff = a - b;
	}

	return min_slope_id;
}

unsigned int GridLees::heuristicClose(unsigned int id) {
	Coord a = paths_[id].start;
	Coord c = paths_[id].end;
	double min = INFINITY;
	unsigned int min_id = 0;

	for (int i = 0; i < paths_.size(); ++i) {
		if (i != id) {
			Coord b = paths_[i].start;
			Coord d = paths_[i].end;
			double diff = (length(a.x, a.y, b.x, b.y) + length(c.x, c.y, d.x, d.y)) / 2;
			if (diff < min) {
				min = diff;
				min_id = i;
			}
		}
	}

	return min_id;
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
		printArea(start_, end_);
        clear();

        return false;
    }
    else {        
        if (!calculatePath(wire_id)) {
			getGrid(start_.x_nearest, start_.y_nearest, start_.z) = CELL_PIN;
			if (debug_mode_ & DEBUG_MID_PATHS)
					printPath(wire_id);
			clear();
            return false;
        }

		if (debug_mode_ & DEBUG_MID_PATHS)
			printPath(wire_id);
        
    }

	getGrid(start_.x_nearest, start_.y_nearest, start_.z) = CELL_PIN;

    //clearQueue(visiting_);
    clear();

    return true;
}

void GridLees::rip(unsigned int wire) {
    Path p = paths_[wire];
    paths_.erase(paths_.begin() + wire);
    paths_.push_back(p);
}

void GridLees::printArea(Coord min_coord, Coord max_coord) {
	return;
	std::cout << "Printing Area from:\n";
	std::cout << "\t" << min_coord.print() << "\n";
	std::cout << "\t" << max_coord.print() << "\n";

	for (int z = 0; z < depth_; ++z) {
		std::cout << min_coord.x << ", " << min_coord.y << " - " << max_coord.x << ", " << max_coord.y << "\n";
		Coord min = min_coord;
		min.z = z;
		getLocalPos(min);
		Coord max = max_coord;
		max.z = z;
		getLocalPos(max);
		int x1 = min.x_nearest;
		int x2 = max.x_nearest;
		int y1 = min.y_nearest;
		int y2 = max.y_nearest;

		if (x1 > x2) {
			int t = x1;
			x1 = x2;
			x2 = t;
		}

		if (y1 > y2) {
			int t = y1;
			y1 = y2;
			y2 = t;
		}

		int mx = layers_[z].width - 1;
		int my = layers_[z].length - 1;
		x1 = (x1 - 10 < 0) ? 0 : x1 - 10;
		x2 = (x2 + 10 > mx) ? mx : x2 + 10;
		y1 = (y1 - 10 < 0) ? 0 : y1 - 10;
		y2 = (y2 + 10 > my) ? my : y2 + 10;

		std::cout << x1 << ", " << y1 << " - " << x2 << ", " << y2 << "\n";

		for (int y = y1; y <= y2; ++y) {
			for (int x = x1; x <= x2; ++x) {
				std::string str;
				Cell val = getGrid(x, y, z);
				
				if (val == CELL_PIN) {
					str = "pppp";
				}
				else if (val == CELL_EMPTY) {
					str = "____";
				}
				else if (val == CELL_BLOCK) {
					str = "####";
				}
				else if (val <= CELL_BASE_WIRE_BLOCK) {
					int fix_val = CELL_BASE_WIRE_BLOCK - val;
					fix_val = paths_[fix_val].id;
					str = "w" + std::to_string(fix_val);
				}
				else {
					str = std::to_string(val);
				}

				std::cout << std::setw(4) << str << " ";
			}
			std::cout << "\n";
		}
		std::cout << "-------\n";
	}
}

bool GridLees::route() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
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
            //if (i < 1) {
				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
				unsigned long time = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
				std::cout << "Failed after " << time << " seconds.\n";

                return false;
            //}

			unsigned int id = heuristicSlope(i);
			//heuristicSlope(i);
            rip(id);
            std::cout << "Failed; Ripping path: " << paths_[id].print() << "\n";

            // Clear Pin Paths
            clearRoute(i);
            clearRoute(id);

            // Redo the current route. (-1 to repeat, another -1 because we ripped a node.)
            i -= 2;
        }
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	unsigned long time = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
	std::cout << "Success after " << time << " seconds.\n";

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
	return;

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
	std::string val,lastval;
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
					val=lastval;
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

// Get List of Paths
std::vector<OutputPath> GridLees::getPaths() {
	std::vector<OutputPath> paths;
	paths.resize(paths_.size());

	// For every path
	for (int p = 0; p < paths_.size(); ++p) { // paths_.size()
		OutputPath outpath;
		// Get the path ID
		int wire_id = wireToCell(p);

		// Get the start and end...
		Coord start = paths_[p].start;
		Coord end;
		Coord finalpos = paths_[p].end;

		int x, y, z;
		x = start.x_nearest;
		y = start.y_nearest;
		z = start.z;

		Layer &l = layers_[z];

		bool findingpath = true;
		// ...trace from start to end.
		while (findingpath) {
			findingpath = false;
			OutputSegment outsegment;
			outsegment.layer = z;

			// Get accurate start position
			start.x = x;
			start.y = y;
			getGlobalPos(start);
			outsegment.startx = start.x;
			outsegment.starty = start.y;

			if (layers_[z].is_horizontal) {
				while (x > 0 && getGrid(x - 1, y, z) == wire_id) {
					getGrid(x - 1, y, z) = CELL_BLOCK;
					--x;
					findingpath = true;
				}
				while (x < width_ - 1 && getGrid(x + 1, y, z) == wire_id) {
					getGrid(x + 1, y, z) = CELL_BLOCK;
					++x;
					findingpath = true;
				}
			}
			else {
				while (y > 0 && getGrid(x, y - 1, z) == wire_id) {
					getGrid(x, y - 1, z) = CELL_BLOCK;
					--y;
					findingpath = true;
				}
				while (y < length_ - 1 && getGrid(x, y + 1, z) == wire_id) {
					getGrid(x, y + 1, z) = CELL_BLOCK;
					++y;
					findingpath = true;
				}
			}

			// Get accurate end path
			end.x = x;
			end.y = y;
			getGlobalPos(end);
			outsegment.endx = end.x;
			outsegment.endy = end.y;

			outsegment.zdir = NoZ;
			if (z > 0) {
				unsigned int new_x;
				unsigned int new_y;
				if (canGoToLayer(x, y, z, z - 1, new_x, new_y)) {
					if (getGrid(new_x, new_y, z - 1) == wire_id) {
						getGrid(new_x, new_y, z - 1) = CELL_BLOCK;
						x = new_x;
						y = new_y;
						--z;
						outsegment.zdir = In;
						findingpath = true;
					}
				}
			}

			if (outsegment.zdir == NoZ && z < depth_ - 1) {
				unsigned int new_x;
				unsigned int new_y;
				if (canGoToLayer(x, y, z, z + 1, new_x, new_y)) {
					if (getGrid(new_x, new_y, z + 1) == wire_id) {
						getGrid(new_x, new_y, z + 1) = CELL_BLOCK;
						x = new_x;
						y = new_y;
						++z;
						outsegment.zdir = Out;
						findingpath = true;
					}
				}
			}

			//std::cout << "Moved to " << x << ", " << y << ", " << z << "\n";

			outpath.paths.push_back(outsegment);

			if (x == finalpos.x_nearest && y == finalpos.y_nearest && z == finalpos.z) {
				break;
			}
		}

		paths[paths_[p].id] = outpath;
	}

	return paths;
}


