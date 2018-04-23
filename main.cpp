#include <iostream>
#include "Grid.hpp"

int main() {
    Grid grid(10, 8);
    grid.setStart(2, 1);
    grid.setEnd(6, 5);

    grid.setBlock(4, 4);
    grid.setBlock(4, 5);
    grid.setBlock(4, 6);
    grid.setBlock(5, 4);
    grid.setBlock(6, 4);

    grid.simulate();

    return 0;
}