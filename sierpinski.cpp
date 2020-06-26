#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <time.h>
#include <vector>
#include <algorithm>
#include "omp.h"
using namespace std;

const int iXmax = 1024;
const int iYmax = 1024;
int depth = 8;
const int MaxColorComponentValue=255;
unsigned char colo[iXmax][iYmax][3];

void putpixel(int x, int y,int r, int g, int b);
void bresenham_line(int x1, int y1, int x2, int y2);
void subTriangle(int n, vector<int> cs);
void draw_triangle(vector<int> cs);
vector<int> get_next_coords(vector<int> entry, int direction);

// Useful: g++ main.cpp -o main -fopenmp
// image open: ristretto image.ppm
int main(int c, char **v)
{
	srand(time(NULL));
    FILE * fp = fopen("sierp.ppm","wb"); /* b -  binary mode */
    fprintf(fp,"P6\n #\n %d\n %d\n %d\n",iXmax,iYmax,MaxColorComponentValue);

    // Initial coords:x1     y1       x2          y2            x3       y3
    vector<int> cs = {10, iYmax - 10, iXmax - 10, iYmax - 10, iXmax / 2, 10};
    // Draw main triangle and call recursive function which splits into 3 triangles
    draw_triangle(cs);
    subTriangle(depth, get_next_coords(cs, 0));

    // output to the file
	fwrite(colo,1,sizeof(colo),fp);
	return 0;
}

void draw_triangle(vector<int> cs)
{
	// Draw the 3 sides of the triangle as lines
	bresenham_line(cs.at(0), cs.at(1), cs.at(2), cs.at(3));
	bresenham_line(cs.at(0), cs.at(1), cs.at(4), cs.at(5));
	bresenham_line(cs.at(2), cs.at(3), cs.at(4), cs.at(5));
}

void subTriangle(int depth, vector<int> cs)
{
    draw_triangle(cs);

	// Calls itself 3 times with new corners, but only if 
	// the current number of recursions is smaller than the maximum depth
	if(depth > 0)
	{
		#pragma omp parallel
		{
			#pragma omp single
			{
				#pragma omp task
				{subTriangle(depth-1, get_next_coords(cs, 1));}
				
				#pragma omp task
				{subTriangle(depth-1, get_next_coords(cs, 2));}
				
				#pragma omp task
				{subTriangle(depth-1, get_next_coords(cs, 3));}

				#pragma omp taskwait
			}
		}
	}
}

void bresenham_line(int x1,int y1,int x2,int y2)
{
	int r = (rand()%254)+100;
	int g = (rand()%254)+100;
	int b = (rand()%254)+100;
	int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
	dx=x2-x1; dy=y2-y1;dx1=fabs(dx); dy1=fabs(dy);
	px=2*dy1-dx1; py=2*dx1-dy1;

	if(dy1<=dx1)
	{  
		if(dx>=0){ x=x1; y=y1;xe=x2;}
		else {x=x2; y=y2; xe=x1;}
		putpixel(x,y,r,g,b);
	for(i=0;x<xe;i++)
	{
			x=x+1;
			if(px<0){ px=px+2*dy1;}
			else{if((dx<0 && dy<0) || (dx>0 && dy>0)){y=y+1;}
			 else{y=y-1;}
			 px=px+2*(dy1-dx1);}
			putpixel(x,y,r,g,b);
		}
	}
	else
	{
		if(dy>=0){x=x1;y=y1;ye=y2;}
		else{x=x2;y=y2;ye=y1;}
		putpixel(x,y,r,g,b);
		for(i=0;y<ye;i++)
		{
			y=y+1;
			if(py<=0){py=py+2*dx1;}
			else{if((dx<0 && dy<0) || (dx>0 && dy>0)){x=x+1;}
			 else{x=x-1;}
			 py=py+2*(dx1-dy1);}
			putpixel(x,y,r,g,b);
		}
	}
}

void putpixel(int x, int y,int r, int g, int b)
{
	colo[x][y][0]=r; /* Red*/
	colo[x][y][1]=g;  /* Green */
	colo[x][y][2]=b;/* Blue */
}

vector<int> get_next_coords(vector<int> cs, int direction)
{
	if (direction == 0)
	{
		cs = 		{(cs.at(0) + cs.at(2))/2, //x coordinate of first corner
    				 (cs.at(1) + cs.at(3))/2,  //y coordinate of first corner
    				 (cs.at(0) + cs.at(4))/2,  //x coordinate of second corner
    				 (cs.at(1) + cs.at(5))/2,  //y coordinate of second corner
    				 (cs.at(2) + cs.at(4))/2,  //x coordinate of third corner
    				 (cs.at(3) + cs.at(5))/2}; //y coordinate of third corner
	}
	else if (direction == 1)
	{
		cs = 		{(cs.at(0) + cs.at(2)) / 2 + (cs.at(2) - cs.at(4)) / 2, 
    				 (cs.at(1) + cs.at(3)) / 2 + (cs.at(3) - cs.at(5)) / 2,  
    				 (cs.at(0) + cs.at(2)) / 2 + (cs.at(0) - cs.at(4)) / 2,  
    				 (cs.at(1) + cs.at(3)) / 2 + (cs.at(1) - cs.at(5)) / 2,  
    				 (cs.at(0) + cs.at(2)) / 2, 
    				 (cs.at(1) + cs.at(3)) / 2}; 
	}
	else if (direction == 2)
	{
		cs = 		{(cs.at(4) + cs.at(2)) / 2 + (cs.at(2) - cs.at(0)) / 2,
    				 (cs.at(5) + cs.at(3)) / 2 + (cs.at(3) - cs.at(1)) / 2,
    				 (cs.at(4) + cs.at(2)) / 2 + (cs.at(4) - cs.at(0)) / 2,
    				 (cs.at(5) + cs.at(3)) / 2 + (cs.at(5) - cs.at(1)) / 2,
    				 (cs.at(4) + cs.at(2)) / 2,
    				 (cs.at(5) + cs.at(3)) / 2};
	}
	else if (direction == 3)
	{
		cs = 		{(cs.at(0) + cs.at(4)) / 2 + (cs.at(4) - cs.at(2)) / 2,
    				 (cs.at(1) + cs.at(5)) / 2 + (cs.at(5) - cs.at(3)) / 2, 
    				 (cs.at(0) + cs.at(4)) / 2 + (cs.at(0) - cs.at(2)) / 2, 
    				 (cs.at(1) + cs.at(5)) / 2 + (cs.at(1) - cs.at(3)) / 2, 
    				 (cs.at(0) + cs.at(4)) / 2, 
    				 (cs.at(1) + cs.at(5)) / 2};
	}

	return cs;
}

