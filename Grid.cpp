#include "Grid.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <iomanip>

const bool DEBUG_PATH = false;

CellID::CellID(int x, int y, int dist) {
    this->x = x;
    this->y = y;
    this->dist = dist;
}

Grid::Grid(int w, int l) : width_(w), length_(l) {
    if (w < 0 || l < 0) {
        throw std::runtime_error("Invalid Size for Grid.");
    }

    grid_ = new Cell *[l];

    for (int i = 0; i < l; ++i) {
        grid_[i] = new Cell[w];

        for (int j = 0; j < w; ++j) {
            grid_[i][j] = -1;
        }
    }
}

void Grid::setStart(int x, int y) {
    xstart_ = x;
    ystart_ = y;
    grid_[y][x] = 0;
}

void Grid::setEnd(int x, int y) {
    xend_ = x;
    yend_ = y;
    grid_[y][x] = -2;
}

void Grid::setBlock(int x, int y) {
    grid_[y][x] = -3;
}

bool Grid::checkValue(Cell val) {
    return (val == -1);
}

bool Grid::checkCell(int x, int y, Check check) {
    switch (check) {
        case CHECK_UP:
            if (y < length_ - 1)
                return checkValue(grid_[y + 1][x]);
        case CHECK_DOWN:
            if (y > 0)
                return checkValue(grid_[y - 1][x]);
        case CHECK_RIGHT:
            if (x < width_ - 1)
                return checkValue(grid_[y][x + 1]);
        case CHECK_LEFT:
            if (x > 0)
                return checkValue(grid_[y][x - 1]);
    }

    return false;
}

bool Grid::simulateStep() {
    CellID id;
    id = visiting_.front();
    visiting_.pop();

    if (id.x == xend_ && id.y == yend_) {
        return false;
    }
    else {
        int v = grid_[id.y][id.x] + 1;

        if (id.x > 0) {
            if (checkValue(grid_[id.y][id.x - 1])) {
                visiting_.emplace(id.x - 1, id.y, v);
                grid_[id.y][id.x - 1] = v;
            }
            else if (grid_[id.y][id.x - 1] == -2) {
                return false;
            }
        }

        if (id.x < width_ - 1) {
            if (checkValue(grid_[id.y][id.x + 1])) {
                visiting_.emplace(id.x + 1, id.y, v);
                grid_[id.y][id.x + 1] = v;
            }
            else if (grid_[id.y][id.x + 1] == -2) {
                return false;
            }
        }

        if (id.y > 0) {
            if (checkValue(grid_[id.y - 1][id.x])) {
                visiting_.emplace(id.x, id.y - 1, v);
                grid_[id.y - 1][id.x] = v;
            }
            else if (grid_[id.y - 1][id.x] == -2) {
                return false;
            }
        }

        if (id.y < length_ - 1) {
            if (checkValue(grid_[id.y + 1][id.x])) {
                visiting_.emplace(id.x, id.y + 1, v);
                grid_[id.y + 1][id.x] = v;
            }
            else if (grid_[id.y + 1][id.x] == -2) {
                return false;
            }
        }
    }

    if (DEBUG_PATH) {
        printGrid();
        std::cout << "_______________\n";
    }

    return true;
}

void Grid::calculatePath() {
    visiting_.empty();

    std::vector<CellID> path;

    int x = xend_;
    int y = yend_;
    int min = INT32_MAX;
    while (true) {
        int xmin = x;
        int ymin = y;

        if (x > 0 && grid_[y][x - 1] >= 0 && grid_[y][x - 1] < min) {
            path.emplace_back(x - 1, y, 0);
            min = grid_[y][x - 1];
            xmin = x - 1;
            ymin = y;
        }

        if (x < width_ - 1 && grid_[y][x + 1] >= 0 && grid_[y][x + 1] < min) {
            min = grid_[y][x + 1];
            xmin = x + 1;
            ymin = y;
        }

        if (y > 0 && grid_[y - 1][x] >= 0 && grid_[y - 1][x] < min) {
            min = grid_[y - 1][x];
            xmin = x;
            ymin = y - 1;
        }

        if (y < length_ - 1 && grid_[y + 1][x] >= 0 && grid_[y + 1][x] < min) {
            min = grid_[y + 1][x];
            xmin = x;
            ymin = y + 1;
        }
        
        if (min == 0) {
            break;
        }

        path.emplace_back(xmin, ymin, 0);

        x = xmin;
        y = ymin;
    }

    for (int i = 0; i < length_; ++i) {
        for (int j = 0; j < width_; ++j) {
            bool found = false;
            for (int k = 0; k < path.size(); ++k) {
                CellID id = path[k];

                if (id.x == j && id.y == i) {
                    found = true;
                    break;
                }
            }

            if (found) {
                std::cout << "x ";
            }
            else if (grid_[i][j] == 0) {
                std::cout << "s ";
            }
            else if (grid_[i][j] == -2) {
                std::cout << "e ";
            }
            else if (grid_[i][j] == -3) {
                std::cout << "# ";
            }
            else {
                std::cout << "_ ";
            }
        }

        std::cout << "\n";
    }
}

void Grid::simulate() {
    int timeout = 80;

    visiting_.emplace(xstart_, ystart_, 0);

    // Wait for timeout, in case of bugs
    while(visiting_.size() > 0 && simulateStep() && --timeout > 0);

    if (!DEBUG_PATH) {
        printGrid();
    }

    if (visiting_.size() == 0) {
        std::cout << "Could not find path to end!\n";
    }
    else if (timeout == 0) {
        std::cout << "Timeout! Failed to find end in time.\n";
    }
    else {
        std::cout << "Found End! Tacking...\n";

        calculatePath();
    }
}

std::string Grid::printSymbol(Cell val) {
    switch(val) {
    default:
        return std::to_string(val);
    case -1:
        return "__";
    case 0:
        return "ss";
    case -2:
        return "ee";
    case -3:
        return "##";
    }
}

void Grid::printGrid() {
    for (int i = 0; i < length_; ++i) {
        for (int j = 0; j < width_; ++j) {
            std::cout << std::setw(2) << printSymbol(grid_[i][j]) << " ";
        }
        std::cout << "\n";
    }
}

Grid::~Grid() {
    if (grid_ != nullptr) {
        /*for (int i = 0; i < length_; ++i) {
            delete[] grid_[i];
        }*/

        delete[] grid_;
    }
}