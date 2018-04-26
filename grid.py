import sys
from Queue import *

class Grid:
    """Grid Class"""

    width_ = 0
    length_ = 0
    xstart_ = 0
    ystart_ = 0
    xend_ = 0
    yend_ = 0
    grid_ = []
    visiting_ = Queue(1000)

    def __init__(self, width = 8, length = 8):
        self.width_ = width
        self.length_ = length

        self.grid_ = [[-1 for x in xrange(width)] for y in xrange(length)] 
        
    def setStart(self, x, y):
        if (x < 0 or x > self.width_):
            raise ValueError("x value of setBegin out of bounds.")
        elif (y < 0 or y > self.length_):
            raise ValueError("y value of setBegin out of bounds.")
        else:
            self.xstart_ = x
            self.ystart_ = y
            self.grid_[y][x] = 0

    def setEnd(self, x, y):
        if (x < 0 or x > self.width_):
            raise ValueError("x value of setEnd out of bounds.")
        elif (y < 0 or y > self.length_):
            raise ValueError("y value of setEnd out of bounds.")
        else:
            self.xend_ = x
            self.yend_ = y
            self.grid_[y][x] = -2

    def simulate(self):
        print("Simulating...")
        timeout = 80

        self.visiting_.put({ 'x': self.xstart_, 'y': self.ystart_ })

        searching = True
        
        # Wait for timeout, in case of bugs
        while(not self.visiting_.empty() and searching and timeout > 0):
            searching = self.simulateStep()
            timeout -= 1
        
        self.printGrid()

        if (self.visiting_.empty()):
            print("Could not find path to end!")
        elif (timeout == 0):
            print("Timeout! Failed to find end in time.")
        else:
            print("Found End! Calculating Path...")
            self.calculatePath()

    def simulateStep(self):
        id = self.visiting_.get()

        if (id['x'] == self.xend_ and id['y'] == self.yend_):
            return False
        else:
            v = self.grid_[id['y']][id['x']] + 1

            if (id['x'] > 0):
                if (self.checkValue(self.grid_[id['y']][id['x'] - 1])):
                    self.visiting_.put({'x': id['x'] - 1, 'y': id['y']})
                    self.grid_[id['y']][id['x'] - 1] = v
                elif (self.grid_[id['y']][id['x'] - 1] == -2):
                    return False

            if (id['x'] < self.width_ - 1):
                if (self.checkValue(self.grid_[id['y']][id['x'] + 1])):
                    self.visiting_.put({'x': id['x'] + 1, 'y': id['y']})
                    self.grid_[id['y']][id['x'] + 1] = v
                elif (self.grid_[id['y']][id['x'] + 1] == -2):
                    return False

            if (id['y'] > 0):
                if (self.checkValue(self.grid_[id['y'] - 1][id['x']])):
                    self.visiting_.put({'x': id['x'], 'y': id['y'] - 1})
                    self.grid_[id['y'] - 1][id['x']] = v
                elif (self.grid_[id['y'] - 1][id['x']] == -2):
                    return False

            if (id['y'] < self.length_ - 1):
                if (self.checkValue(self.grid_[id['y'] + 1][id['x']])):
                    self.visiting_.put({'x': id['x'], 'y': id['y'] + 1})
                    self.grid_[id['y'] + 1][id['x']] = v
                elif (self.grid_[id['y'] + 1][id['x']] == -2):
                    return False
            
        return True

    def printGrid(self):
        print("Printing Grid")

        for i in range(0, self.length_):
            line = ""

            for j in range(0, self.width_):
                v = self.printSymbol(self.grid_[i][j]) 
                line += v + " "
            
            print(line)

        print("-------------")

    def printSymbol(self, val):
        switch = {
            -1: "__",
            0:  "ss",
            -2: "ee",
            -3: "##"
        }
        return switch.get(val, "{0:0=2d}".format(val))

    def checkCell(self, x, y, check):
        if (check == 0 and y < self.length_ - 1):
            return self.checkValue(self.grid_[y + 1][x])
        if (check == 1 and y > 0):
            return self.checkValue(self.grid_[y - 1][x])
        if (check == 2 and x < self.width_ - 1):
            return self.checkValue(self.grid_[y][x + 1])
        if (check == 3 and x > 0):
            return self.checkValue(self.grid_[y][x - 1])

        return False

    def checkValue(self, val):
        return (val == -1)

    def setBlock(self, x, y):
        if (x < 0 or x > self.width_):
            raise ValueError("x value of setBlock out of bounds.")
        elif (y < 0 or y > self.length_):
            raise ValueError("y value of setBlock out of bounds.")
        else:
            self.grid_[x][y] = -3

    def calculatePath(self):
        print("Calculating Path.")
        path = []

        x = self.xend_
        y = self.yend_
        min = sys.maxint

        processing = True
        while (processing):
            xmin = x
            ymin = y

            if (x > 0 and self.grid_[y][x - 1] >= 0 and self.grid_[y][x - 1] < min):
                path.append({'x': x - 1, 'y': y})
                min = self.grid_[y][x - 1]
                xmin = x - 1
                ymin = y

            if (x < self.width_ - 1 and self.grid_[y][x + 1] >= 0 and self.grid_[y][x + 1] < min):
                min = self.grid_[y][x + 1]
                xmin = x + 1
                ymin = y

            if (y > 0 and self.grid_[y - 1][x] >= 0 and self.grid_[y - 1][x] < min):
                min = self.grid_[y - 1][x]
                xmin = x
                ymin = y - 1

            if (y < self.length_ - 1 and self.grid_[y + 1][x] >= 0 and self.grid_[y + 1][x] < min):
                min = self.grid_[y + 1][x]
                xmin = x
                ymin = y + 1
            
            if (min == 0):
                processing = False
            else:
                path.append({'x': xmin, 'y': ymin})

                x = xmin
                y = ymin

        for i in range(0, self.length_):
            line = ""
            for j in range(0, self.width_):
                found = False
                for k in range(0, len(path)):
                    id = path[k]

                    if (id['x'] == j and id['y'] == i):
                        found = True
                        break

                if (found):
                    line += "x "
                elif (self.grid_[i][j] == 0):
                    line += "s "
                elif (self.grid_[i][j] == -2):
                    line += "e "
                elif (self.grid_[i][j] == -3):
                    line += "# "
                else:
                    line += "_ "

            print(line)

