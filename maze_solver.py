from collections import namedtuple
from itertools import combinations, product
from random import randint, choice
import matplotlib.pyplot as plt
from numpy import ones, uint16

# structure containing width and height used by Maze class
sizetuple = namedtuple('size', ['w','h'])

class Maze:
    def __init__(self):
        pass
    # Generates an array of given size that is later filled with pixels of generated maze
    # Array can have these values
    # 1 - wall
    # 0 - corridor
    # todo: create a choice whether white is corridor or black?
    # todo: start and end corner as params
    def generate(self, width, height):
        # Size of maze cannot be even (problem with drawing boundaries)
        width = width if width % 2 != 0 else width + 1
        height = height if height % 2 != 0 else height + 1

        # Generate array with just 1's
        self.size = sizetuple(w=width, h=height)
        self.mazearr = ones(shape=self.size, dtype=uint16)

        # Put entry and exit
        self.mazearr[[1,1], [1,0]] = 0
        self.mazearr[self.size.w-2,self.size.h-1] = 0

        # Randomly carve the maze
        # Starting at index (x,y) = (0,0)
        self.carveMaze(1, 1)

        fig = plt.figure()
        plt.imshow(self.mazearr, cmap='Greys', interpolation='none')
        plt.show()
    #

    # Method used by generate to randomly carve corridors in the maze
    # x, y => start point of carving maze
    def carveMaze(self, x_s, y_s):
        # possible moves to carve
        dirs = [(1, 0), (0, 1), (-1, 0), (0, -1)]
        # Iterate over maze
        for x, y in product(range(x_s, self.size.h, 2), range(y_s, self.size.w, 2)):
            dx,dy = choice(dirs)
            count = 0
            while count < 4:
                # Assign new coordinates to move carving point
                x1 = x + dx; y1 = y + dy
                x2 = x1 + dx; y2 = y1 + dy
                # If new coords are inside the maze and it has not been carved there before
                if x2 in range(1, self.size.w) and y2 in range(1, self.size.h) \
                        and all(self.mazearr[[y1,y2], [x1,x2]] == 1):
                    # Carve maze and choose new direction
                    self.mazearr[[y1,y2], [x1,x2]] = 0
                    x = x2; y = y2
                    dx, dy = choice(dirs)
                    count = 0
                else:
                    dx,dy = dirs[(dirs.index((dx,dy)) + 1) % len(dirs)]
                    count += 1
    #

if __name__ == "__main__":
    maze = Maze()
    maze.generate(30, 30)
    # maze.generate(200, 200)
