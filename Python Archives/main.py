#!/usr/bin/env python

import sys
from grid import Grid

def main(args):
    try:
        grid = Grid(10, 8)

        grid.setStart(2, 1)
        grid.setEnd(6, 5)

        grid.setBlock(4, 4)
        grid.setBlock(4, 5)
        grid.setBlock(4, 6)
        grid.setBlock(5, 4)
        grid.setBlock(6, 4)

        grid.simulate()
    except ValueError as e:
        print(e)


if __name__ == "__main__":
    main(sys.argv)