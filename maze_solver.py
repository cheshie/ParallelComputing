from collections import namedtuple, deque
from itertools import combinations, product
from random import randint, choice
import matplotlib.pyplot as plt
from numpy import ones, uint16, where, zeros, array, ndarray
from threading import Thread, Lock

# structure containing width and height used by Maze class
sizetuple = namedtuple('size', ['w','h'])

class Maze:
    def __init__(self):
        self.mutex1 = Lock()
        self.count3 = 1
    # Generates an array of given size that is later filled with pixels of generated maze
    # Array can have these values
    # -1 - wall
    # 0 - corridor
    def generate(self, width, height):
        # Size of maze cannot be even (problem with drawing boundaries)
        width = width if width % 2 != 0 else width + 1
        height = height if height % 2 != 0 else height + 1

        # Generate array with just 1's
        self.size = sizetuple(w=width, h=height)
        # Array containing maze
        self.mazearr = ones(shape=self.size) * - 1
        # Array of values (r,g,b)
        self.rgbmaze = zeros(shape=self.size, dtype=(int, 3))
        # Array of mutexes - here is just symbolic, because Python is not
        # capable of true multiprocessing. More:
        # https://stackoverflow.com/questions/3310049/proper-use-of-mutexes-in-python
        self.mtable = [[Lock() for x in range(self.size[0])] for x in range(self.size[1])]

        # Put entry and exit
        self.mazearr[[1,1], [1,0]] = 0
        self.mazearr[self.size.w-2,self.size.h-1] = 0

        # Randomly carve the maze
        # Starting at index (x,y) = (0,0)
        self.carveMaze(1, 1)
        self.rgbmaze[where(self.mazearr == 0)] = (255, 255, 255)
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
                        and all(self.mazearr[[y1,y2], [x1,x2]] == -1):
                    # Carve maze and choose new direction
                    self.mazearr[[y1,y2], [x1,x2]] = 0
                    x = x2; y = y2
                    dx, dy = choice(dirs)
                    count = 0
                else:
                    dx,dy = dirs[(dirs.index((dx,dy)) + 1) % len(dirs)]
                    count += 1
    #

    def plotMaze(self):
        fig = plt.figure("Maze solver")
        plt.suptitle("Each corridor - different color")
        plt.imshow(self.rgbmaze)#, cmap='Greys', interpolation='none')
        plt.show()
    #

    # Generate random color for particular thread walking a corridor
    def rgb_rand(self):
        return (randint(0,255),randint(0,255), randint(0,255))
    #

    def solveMaze(self):
        # Start main thread that enters the maze and later will split into more threads
        mainThread = Thread(target=maze.solvingAlg, args=(1, 0, maze.rgb_rand(),))
        mainThread.start()
        mainThread.join()
    #

    def solvingAlg(self, x, y, color):
        # que of threads that will follow different junctions
        explorers = deque()
        # Last position in maze of the thread
        lastpos = (0,0)

        def test(x,y):
            # Check if x,y are in the maze, if that position in maze is not wall and
            # also whether it is not locked and if it is not the last position of that particular thread
            if x in range(self.size[0]) and y in range(self.size[1]) and not self.mazearr[x, y] and\
                self.mtable[x][y].acquire(False) and lastpos != (x, y):
                return True
            return False
        #

        while True:
            # Walk corridor
            self.rgbmaze[x, y] = color
            # Check in four directions whether it is possible to walk or not
            # notice: simple loop could be used here, but threading did not like that loop here
            moves = deque()
            dirs = [(x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)]
            if test(*dirs[0]): moves.append((x - 1, y))
            if test(*dirs[1]): moves.append((x + 1, y))
            if test(*dirs[2]): moves.append((x, y - 1))
            if test(*dirs[3]): moves.append((x, y + 1))

            if not moves: break
            # Add new position, assign new x,y and pop one direction from moves list
            lastpos = x, y; x, y = moves[0]; moves.popleft()
            # For the remaning moves, fire up new threads
            for i, exp in enumerate(moves):
                explorers.append(Thread(target=self.solvingAlg, args=(*moves[i], self.rgb_rand(),)))
                explorers[-1].start()

        for x in explorers: x.join()
    #
#

if __name__ == "__main__":
    maze = Maze()
    maze.generate(200, 200)
    maze.solveMaze()
    maze.plotMaze()
