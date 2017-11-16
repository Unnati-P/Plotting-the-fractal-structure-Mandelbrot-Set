/*
   Serial implementation of plotting fractals using Mandelbrot Set
 */

# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <omp.h>


//  Using the MONOTONIC clock 
#define CLK CLOCK_MONOTONIC
#define RGB_COMPONENT_COLOR 255

typedef struct color{
  int r,g,b;
} color;

typedef struct {
  unsigned char red,green,blue;
} PPMPixel;

typedef struct {
  int x, y; 
  PPMPixel *data;
} PPMImage;

typedef struct {
  unsigned char gs;
} PPMPixelGS;


typedef struct {
  int x, y;
  PPMPixelGS *data;
} PPMImageGS;

/* Function to compute the difference between two points in time */
struct timespec diff(struct timespec start, struct timespec end);
/* Function to write the header details in the output PPM file */
void writePPM(const char *filename, PPMImage* img);
/* Function to get colour of pixel based on the number of iterations at which the value converges */
color* getColour(int iterations);
/* Function for calculating Mandelbrot values for each pixel for count_max iterations and plotting the corresponding r,g,b values in the output image */
PPMImage* plot();

  int rows = 256;
  int cols = 256;
  int c;
  int c_max;
  int count_max = 15000;		// max number iterations for which we calculate the Mandelbrot function values for each pixel
  int i;
  int j;
  int k;
  double wtime;
  double wtime_total;
  double x_max =   1.25;	// choosing X-scale to scale our input points
  double x_min = - 2.25;
  double x;
  double x2;
  double y_max =   1.75;	// choosing X-scale to scale our input points
  double y_min = - 1.75;
  double y;
  double y2;

/* 
   Function to computes the difference between two time instances
*/
struct timespec diff(struct timespec start, struct timespec end){
	struct timespec temp;
	if((end.tv_nsec-start.tv_nsec)<0){
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	}
	else{
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}


/*
  Function to write the header details in the output PPM file
*/
void writePPM(const char *filename, PPMImage* img) {
  FILE *fp;
  //open file for output
  fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  //write the header file
  //image format
  fprintf(fp, "P6\n");

  //comments
  fprintf(fp, "#output file : warping\n");

  //image size
  fprintf(fp, "%d %d\n",img->x,img->y);

  // rgb component depth
  fprintf(fp, "%d\n",255);

  // pixel data
  fwrite(img->data, 3 * img->x, img->y, fp);
  fclose(fp);
}

/*
  Function to get colour of pixel based on the number of iterations at which the value converges
*/
color* getColour(int iterations) {
	color* p = (color*) malloc(sizeof(color));

	if (iterations < count_max && iterations > 0) {

    	int i = iterations % 16;
    
	if(i==0){
		p->r = 66;
		p->g = 30;
		p->b = 15;
	}
	else if(i==1){
		p->r = 25;
		p->g = 7;
		p->b = 26;
	}
	else if(i==2){
		p->r = 9;
		p->g = 1;
		p->b = 47;
	}
	else if(i==3){
		p->r = 4;
		p->g = 4;
		p->b = 73;
	}
	else if(i==4){
		p->r = 0;
		p->g = 7;
		p->b = 100;
	}
	else if(i==5){
		p->r = 12;
		p->g = 44;
		p->b = 138;
	}
	else if(i==6){
		p->r = 24;
		p->g = 82;
		p->b = 177;
	}
	else if(i==7){
		p->r = 57;
		p->g = 125;
		p->b = 209;
	}
	else if(i==8){
		p->r = 134;
		p->g = 181;
		p->b = 229;
	}
	else if(i==9){
		p->r = 211;
		p->g = 236;
		p->b = 248;
	}
	else if(i==10){
		p->r = 241;
		p->g = 233;
		p->b = 191;
	}
	else if(i==11){
		p->r = 248;
		p->g = 201;
		p->b = 95;
	}
	else if(i==12){
		p->r = 255;
		p->g = 170;
		p->b = 0;
	}
	else if(i==13){
		p->r = 204;
		p->g = 128;
		p->b = 0;
	}
	else if(i==14){
		p->r = 153;
		p->g = 87;
		p->b = 0;
	}
	else {
		p->r = 106;
		p->g = 52;
		p->b = 3;
	}
	return p;
}
else{
	p->r = 0;
	p->g = 0;
	p->b = 0;
	return p;
}
}

/*
  Function for calculating Mandelbrot values for each pixel for count_max iterations and plotting the corresponding r,g,b values in the output image
*/
PPMImage* plot() {
 PPMImage *output = (PPMImage *) malloc(sizeof(PPMImage));
  output->x = rows;
  output->y = cols;
  output->data = (PPMPixel *) malloc(rows*cols*sizeof(PPMPixel));
	  double x1;
 double y1;
int count[rows][cols];

for ( i = 0; i < rows; i++ )
  {
    for ( j = 0; j < cols; j++ )
    {
	int converges=0;
      x = ( ( double ) (     j - 1 ) * x_max   
          + ( double ) ( rows - j     ) * x_min ) 
          / ( double ) ( rows     - 1 );

      y = ( ( double ) (     i - 1 ) * y_max   
          + ( double ) ( cols - i     ) * y_min ) 
          / ( double ) ( cols     - 1 );

      count[i][j] = -1;

      x1 = x;
      y1 = y;

      for ( k = 1; k <= count_max; k++ )
      {
        x2 = x1 * x1 - y1 * y1 + x;
        y2 = 2 * x1 * y1 + y;

        if ( x2 < -2.0 || 2.0 < x2 || y2 < -2.0 || 2.0 < y2 )
        {
          count[i][j] = k;
          break;
        }
        x1 = x2;
        y1 = y2;
	
      }

	int idx;
 	idx = rows*i + j;
      	PPMPixel *temp = output->data + idx;
        color* c = getColour(count[i][j]);
       	temp->red= c->r;
        temp->green = c->g;
        temp->blue = c->b;
	
    }
  }

 return output;
} 

int main(int argc, char* argv[])
{
	struct timespec start_e2e, end_e2e, start_alg, end_alg, e2e, alg;
	clock_gettime(CLK, &start_e2e);

	if(argc < 3){
		printf( "Usage: %s n p\n", argv[0] );
		return -1;
	}

	int n=atoi(argv[1]);	//Image pixel size
	int p=atoi(argv[2]);	/* number of processors*/
	char *problem_name = "median_filterng";
	char *approach_name = "qsort";
	char* filename;
	filename = "mandelbrot_ser.ppm";
	
	 
	clock_gettime(CLK, &start_alg);	/* Start the algo timer */

	/*----------------------Core algorithm starts here----------------------------------------------*/
	PPMImage* out = plot();
	/*----------------------Core algorithm finished--------------------------------------------------*/

	clock_gettime(CLK, &end_alg);	/* End the algo timer */
	writePPM(filename,out); 

	clock_gettime(CLK, &end_e2e);
	e2e = diff(start_e2e, end_e2e);
	alg = diff(start_alg, end_alg);

	printf("%s,%s,%d,%d,%ld,%ld,%ld,%ld\n", problem_name, approach_name, n, p, e2e.tv_sec, e2e.tv_nsec, alg.tv_sec, alg.tv_nsec);
	return 0;
}

	

