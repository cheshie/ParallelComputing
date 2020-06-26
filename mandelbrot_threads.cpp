#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <algorithm>
using namespace std;

const int size = 800;
static unsigned char colo[size][size][3];
const int MaxColorComponentValue=255;

void calc_mandelbrot(vector<int> cds);
void putpixel(int x, int y,int r, int g, int b);

// Compile with: g++ main.cpp -o main -lpthread 
int main()
{
    FILE * fp = fopen("mandelbrot_thread.ppm","wb"); /* b -  binary mode */
    fprintf(fp,"P6\n #\n %d\n %d\n %d\n",size,size,MaxColorComponentValue);

    vector<vector<int>> cds;
    std::vector<thread> quarter;
    cds.push_back({0,0,size/2,size/2,0,0,255}); cds.push_back({size/2,0,size,size/2,0,255,0});
    cds.push_back({0,size/2,size/2,size,255,0,0}); cds.push_back({size/2,size/2,size,size,100,100,200});

    for(const auto &c : cds){
        quarter.push_back(thread(calc_mandelbrot, c));
    }

    for(auto &q : quarter){q.join();}    

    // Write to the file
    fwrite(colo,1,sizeof(colo),fp);

    fclose(fp);
    return 0;
}

void calc_mandelbrot(vector<int> cds)
{
    /* screen ( integer) coordinate */
    int iX,iY;

    /* world ( double) coordinate = parameter plane*/
    double Cx,Cy;
    const double CxMin=-2.5; const double CxMax=1.5;
    const double CyMin=-2.0; const double CyMax=2.0;
    double PixelWidth=(CxMax-CxMin)/size, PixelHeight=(CyMax-CyMin)/size;
    /* color component ( R or G or B) is coded from 0 to 255 */
    /* it is 24 bit color RGB file */

    /* Z=Zx+Zy*i  ;   Z0 = 0 */
    double Zx, Zy;
    double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
    int Iteration;
    const int IterationMax=200;
    /* bail-out value , radius of circle ;  */
    const double EscapeRadius=2;
    double ER2=EscapeRadius*EscapeRadius;

    int xc = 0, yc = 0;

    for(iY=0;iY<size;iY++)
    {
        Cy=CyMin + iY*PixelHeight;
        if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */

        for(iX=0;iX<size;iX++)
        {
            Cx=CxMin + iX*PixelWidth;
            /* initial value of orbit = critical point Z= 0 */
            Zx=0.0; Zy=0.0; Zx2=Zx*Zx; Zy2=Zy*Zy;
            for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++){
                Zy=2*Zx*Zy + Cy; Zx=Zx2-Zy2 +Cx; Zx2=Zx*Zx; Zy2=Zy*Zy;}

            if(xc >= cds[0] && xc < cds[2] && yc >= cds[1] && yc < cds[3])//{
                /* compute  pixel color (24 bit = 3 bytes) */
                /*  interior of Mandelbrot set = black */
                if (Iteration==IterationMax){putpixel(xc, yc, 0,0,0);}
                 /* exterior of Mandelbrot set = white */
                else{putpixel(xc, yc, cds[4], cds[5], cds[6]);};
            //}/*write color to the file*/
            xc++;
        }
        xc = 0;yc++;
    }
}

void putpixel(int x, int y,int r, int g, int b)
{
    colo[x][y][0]=r;  /* Red*/
    colo[x][y][1]=g;  /* Green */
    colo[x][y][2]=b;  /* Blue */
}
