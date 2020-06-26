#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "omp.h"
#include <ctime>
#define THREADS 8
using namespace std;
void calc_mandelbrot(unsigned char * colo, int size);

// Compile with: g++ -Wall  main.cpp -o main -fopenmp
int main(int argc, char *argv[])
{   
	srand(time(NULL));
	const int size = 10000; // size of image
    // vector with values of a pixel for current coordinate, initialize with 0 (black)
    unsigned char * colo = new unsigned char[size * size];
    for (int jj = 0; jj < size; jj++) for (int kk = 0; kk < size; kk++) *(colo + jj * size + kk) = 0;

	// For time measurement
	// double start; double end; 
	// start = omp_get_wtime();
   	clock_t begin = clock();	
    calc_mandelbrot(colo, size);
	// end = omp_get_wtime();
	clock_t end = clock();
	printf("[!] Image size: %d took %f seconds\n", size, double(end - begin) / CLOCKS_PER_SEC);
	// printf("[!] Image size: %d took %f seconds\n", size, end - start);

    // File initialization 
    const int MaxColorComponentValue=255;
    FILE * fp = fopen("mandelbrot_omp.ppm","wb");
    fprintf(fp,"P6\n #\n %d\n %d\n %d\n",size,size,MaxColorComponentValue);

    // Table that contains values for pixels: rgb (first row is black - 0,0,0)
    unsigned char rgb_table[THREADS][3];
    rgb_table[0][0] = rgb_table[0][1] = rgb_table[0][2] = 0;
    for (int i=1; i<THREADS; i++)
    {
			rgb_table[i][0] = (rand()%254)+100;
	    	rgb_table[i][1] = (rand()%254)+100;
	    	rgb_table[i][2] = (rand()%254)+100;
    }

    // Write to the file
    for (int i = 0; i < size; i++)
    	for (int j=0; j < size; j++){ 
    		fwrite(rgb_table[(colo + i * size)[j]],1,3,fp);
    	}

    delete[] colo;
    fclose(fp);
    return 0;
}

void calc_mandelbrot(unsigned char * colo, int size)
{   
    // world ( double) coordinate = parameter plane
    const double CxMin=-2.5; const double CxMax=1.5; const double CyMin=-2.0; const double CyMax=2.0;
    // color component ( R or G or B) is coded from 0 to 255
    const int IterationMax=200; const double EscapeRadius=2; // bail-out value , radius of circle;
    double Cx,Cy; double ER2;
    int iX,iY; // screen coordinates
    int iter;
    double PixelWidth, PixelHeight; int Iteration;
    double Zx, Zy; // Z=Zx+Zy*i  ;   Z0 = 0 
    double Zx2, Zy2;
    
    #pragma omp parallel num_threads(THREADS) shared(colo) private(Cx, Cy, ER2, iX, iY, iter, PixelWidth, PixelHeight, Iteration)
    {
    	iX=0; iY=0; iter=0; Iteration=0;
    	PixelWidth=(CxMax-CxMin)/size; PixelHeight=(CyMax-CyMin)/size;
        Cx = 0; Cy = 0;
        ER2 = EscapeRadius*EscapeRadius;
        // int tid=omp_get_thread_num();
        int xc=0, yc=0;
        # if defined omp
        	omp_set_nested(2);
        #endif
        #pragma omp parallel for
        for(iY=0;iY<size;iY++)
        {
            Cy=CyMin + iY*PixelHeight;
            if (fabs(Cy)< PixelHeight/2) Cy=0.0; // Main antenna
            for(iX=0;iX<size;iX++)
            {
            	iter++;
                Cx=CxMin + iX*PixelWidth;
                Zx=0.0; Zy=0.0; Zx2=Zx*Zx; Zy2=Zy*Zy; // initial value of orbit = critical point Z= 0
                for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++){
                    Zy=2*Zx*Zy + Cy; Zx=Zx2-Zy2 +Cx; Zx2=Zx*Zx; Zy2=Zy*Zy;}
                    if (Iteration!=IterationMax){*(colo + xc * size + yc) = 1;}
                xc++;
            }
            xc = 0; yc++;
        }
        printf("Nr of iterations for thread %d: %d\n", 0, iter);
    }
}