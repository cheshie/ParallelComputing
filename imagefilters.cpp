#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <tuple>
#include <chrono>
#include <iostream>
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range2d.h"
using namespace std;

// Size of image
const int iXmax = 512;
const int iYmax = 512;
// Max value of pixel
const int MaxColorComponentValue=255;
// Input and output arrays storing values of pixels <r,g,b>
unsigned char inpt[iXmax][iYmax][3];
unsigned char otpt[iXmax][iYmax][3];

// Filter masks and params
// *************************************************************************
double emboss_kernel[3*3] = {
  -2., -1.,  0.,
  -1.,  1.,  1.,
  0.,  1.,  2.,
};
 
double sharpen_kernel[3*3] = {
  -1.0, -1.0, -1.0,
  -1.0,  9.0, -1.0,
  -1.0, -1.0, -1.0
};
double sobel_emboss_kernel[3*3] = {
  -1., -2., -1.,
  0.,  0.,  0.,
  1.,  2.,  1.,
};
double box_blur_kernel[3*3] = {
  1.0, 1.0, 1.0,
  1.0, 1.0, 1.0,
  1.0, 1.0, 1.0,
};
 
double *filters[4] = {
  emboss_kernel, sharpen_kernel, sobel_emboss_kernel, box_blur_kernel
};

const double filter_params[2*4] = {
  1.0, 0.0,
  1.0, 0.0,
  1.0, 0.5,
  9.0, 0.0
};
// Filter masks and params
// *************************************************************************

// Get particular pixel if coordinates belong to the image
// Otherwise, return 0
unsigned char get_pixel(int x, int y, int l) 
{
  if ( (x<0) || (x >= iXmax) || (y<0) || (y >= iYmax) ) return 0;
  return inpt[x][y][l];
}
// ********

// TBB Task structure
struct filter {
  // Constructor
  // Params: 
	  // _K - type of filter mask
	  // _Ks - nr of type of filter mask
	  // _divisor, _offset - filter parameters
	  // _coords - image coordinates where the filtering is applied to
	  // (used by TBB to split image into threads)
  filter(double *_K, int _Ks, double _divisor, double _offset, vector<int> _coords)
    :K(_K), Ks(_Ks), divisor(_divisor), offset(_offset), coords(_coords)
  {}
  void operator()() 
  {
		int ix, iy, l;
		int kx, ky;
		double cp[3];

		// Filtration algorithm
		for(ix=coords[0]; ix < coords[2]; ix++) 
		{
		  	for(iy=coords[1]; iy < coords[3]; iy++) 
		  	{
				cp[0] = cp[1] = cp[2] = 0.0;
				for(kx=-Ks; kx <= Ks; kx++) 
				{
			  		for(ky=-Ks; ky <= Ks; ky++) 
			  		{
			    		for(l=0; l<3; l++)
			      			cp[l] += (K[(kx+Ks) +
		                        (ky+Ks)*(2*Ks+1)]/divisor) *
		                        ((double)get_pixel(ix+kx, iy+ky, l)) + offset;
			  		}
				}
				// Weights calculation
				for(l=0; l<3; l++)
				  cp[l] = (cp[l]>255.0) ? 255.0 : ((cp[l]<0.0) ? 0.0 : cp[l]) ;

				// Put result in output array
				otpt[ix][iy][0]=(unsigned char) cp[0];  /* Red*/
				otpt[ix][iy][1]=(unsigned char) cp[1];  /* Green */
				otpt[ix][iy][2]=(unsigned char) cp[2];;  /* Blue */
		  	}
		}
  }
  double *K; int Ks; double divisor; double offset; vector<int> coords;
};

// Compile with: g++ -std=c++11 main.cpp -ltbb
int main()
{
	// Input file - read headers
	FILE * fp = fopen("example.ppm","rb"); /* b -  binary mode */
	fprintf(fp,"P6\n #\n %d\n %d\n %d\n",iXmax,iYmax,MaxColorComponentValue);

	// TBB initialization with explicit number of threads
	tbb::task_scheduler_init init(tbb::task_scheduler_init::default_num_threads());
	// Read file into rgb array and copy it
	fread(inpt, 3, sizeof(inpt), fp);
	memcpy(otpt, inpt, sizeof(inpt));
	fclose(fp);

	// Coordinates - in this example 4 quarters of the image (== 4 threads)
	vector<vector<int>> cds;
	cds.push_back({0,0,iXmax/2,iYmax/2}); cds.push_back({iXmax/2,0,iXmax,iYmax/2});
	cds.push_back({0,iYmax/2,iXmax/2,iYmax}); cds.push_back({iXmax/2,iYmax/2,iXmax,iYmax});

	auto start = chrono::steady_clock::now();

	// Initialize vectors
	std::vector<filter> tasks;
	for(const auto &cd : cds) tasks.push_back(filter(filters[1], 1, filter_params[2], filter_params[3], cd));

	// Fire up threads
	tbb::parallel_for(
	tbb::blocked_range<size_t>(0,tasks.size()),
	[&tasks](const tbb::blocked_range<size_t>& r) {
		for (size_t i=r.begin();i<r.end();++i) tasks[i]();
	}
	);

	auto end = chrono::steady_clock::now();

	cout<<"Elapsed time (ms): "<<chrono::duration_cast<chrono::milliseconds>(end - start).count()<<endl;

	// Save filtered image to a output file
	FILE * fotpt = fopen("otpt.ppm","wb"); /* b -  binary mode */
	fprintf(fotpt,"P6\n #\n %d\n %d\n %d\n",iXmax,iYmax,MaxColorComponentValue);
	fwrite(otpt, 3, sizeof(otpt), fotpt);

	fclose(fotpt);
return 0;
}