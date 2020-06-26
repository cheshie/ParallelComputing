# ParallelComputing
Programs for one of my university courses that demonstrate parallel computing using i.e. C++ Threads, OpenMP and TBB

## imagefilters.cpp
Image filtering using Intel's TBB library (ppm images)


## mandelbrot_omp.cpp
Mandelbrot fractal (saved as ppm image) using OpenMP library


## mandelbrot_threads.cpp
Same solution as above, but using C++ 11 Threads


## sierpinski.cpp
Sierpinski carpet (saved as ppm image) using OpenMP library


## ulam.cpp
Ulam spiral (of prime numbers, saved as ppm image) using OpenMP library


## maze_solver.py
Solving randomly generated maze (ppm image) using Python threading library. 
Each junction main thread splits into threads, each of which follows own corridor 
in order to finally reach the exit


