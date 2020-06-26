#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <tuple>
#include "omp.h"
using namespace std;

const int iXmax = 1025;
const int iYmax = 1025;
const int MaxColorComponentValue=255;
unsigned char colo[iXmax][iYmax][3];
int depth = 8;

// Useful: g++ main.cpp -o main -fopenmp
// image open: ristretto image.ppm

int isprime(int n)
{
	int p;
	for (p = 2; p*p <= n; p++)
		if (n%p == 0) return 0;
	return n > 2;
}

int spiral(int w, int h, int x, int y)
{
	return y ? w + spiral(h - 1, w, y - 1, w - x - 1) : x;
}

void putpixel(int x, int y,int r, int g, int b)
{
	colo[x][y][0]=r;  /* Red*/
	colo[x][y][1]=g;  /* Green */
	colo[x][y][2]=b;  /* Blue */
}

int main(int c, char **v)
{
	int i, j, w = iXmax, h = iYmax, s = 1;
	FILE * fp = fopen("ulam.ppm","wb"); /* b -  binary mode */
	fprintf(fp,"P6\n #\n %d\n %d\n %d\n",iXmax,iYmax,MaxColorComponentValue);

	#define WAY1

	#if defined (WAY1)
		// This is way 1:
		// Allow threads to split regions by themselves
		#pragma omp parallel num_threads(4)
		#pragma omp for collapse(2) // Collapse is used for perfectly nested loops
		for (i = 0; i < w; i++)
			 for (j = 0; j < h; j++)
				if (!isprime(w*h + s - 1 - spiral(w, h, j, i)))
					putpixel(i, j, 255, 255, 255);
	#else
		//This is way 2 - manual:
		// Table coordinates: <i_start, j_start, i_end, j_end> for each quarter
		vector<vector<int>> vt;
		vt.push_back({0, 0, h/2, w/2});
		vt.push_back({h/2, 0, h/2 + h/2, w/2});
		vt.push_back({0, w/2, h/2, w/2+w/2});
		vt.push_back({h/2, w/2, h/2+h/2, w/2+w/2});

		//Manually split all threads regions
		#pragma omp parallel num_threads(4)
		{
			int tid = omp_get_thread_num();
			#pragma omp for collapse(2)   // this did not work here: omp_set_nested(2);
			for (i = vt.at(tid).at(0); i < vt.at(tid).at(2); i++)
				for (j = vt.at(tid).at(1); j < vt.at(tid).at(3); j++)
					if (!isprime(w*h + s - 1 - spiral(w, h, j, i)))
						putpixel(i, j, 255, 255, 255);
		}
	#endif		

	// write to the output file
	fwrite(colo,3,sizeof(colo),fp);
	return 0;
}