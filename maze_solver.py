from collections import namedtuple
from random import randint
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
    # todo: start and end corner
    def generate(self, width, height):
        # Generate array with just 1's
        self.size = sizetuple(w=width, h=height)
        self.mazearr = ones(shape=self.size, dtype=uint16)

        # Put entry
        self.mazearr[1][1] = 0

        # Randomly carve the maze
        # Starting at index (x,y) = (0,0)
        self.carveMaze(1, 1)

        fig = plt.figure()
        plt.imshow(self.mazearr, cmap='Greys', interpolation='none')
        plt.show()

    # Method used by generate to randomly carve corridors in the maze
    # x, y => start point of carving maze
    def carveMaze(self, x_s, y_s):
        for x in range(x_s, self.size.h, 2):
            for y in range(y_s, self.size.w, 2):
                direction = randint(0, 4)
                count = 0
                while count < 4:
                    dx = 0; dy = 0
                    # Use some array instead
                    if direction == 0:
                        dx = 1
                    elif direction == 1:
                        dy = 1
                    elif direction == 2:
                        dx = -1
                    else:
                        dy = -1

                    x1 = x + dx
                    y1 = y + dy
                    x2 = x1 + dx
                    y2 = y1 + dy

                    if x2 > 0 and x2 < self.size.w and y2 > 0 and y2 < self.size.h \
                        and self.mazearr[y1][x1] == 1 and self.mazearr[y2][x2] == 1:
                        self.mazearr[y1][x1] = 0
                        self.mazearr[y2][x2] = 0
                        x = x2; y = y2
                        direction = randint(0,4)
                        count = 0
                    else:
                        direction = (direction + 1) % 4
                        count += 1



if __name__ == "__main__":
    maze = Maze()
    maze.generate(15, 15)
    # maze.generate(500, 500)
