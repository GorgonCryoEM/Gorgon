/**************************************
 * Zeyun Yu (zeyun@cs.utexas.edu)    *
 * Department of Computer Science    *
 * University of Texas at Austin     *
 **************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>  
#include <memory.h>

#define SIGMA  3.0
#define WINDOW 3
#define PIE           3.14159265359
#define max2(x, y)     ((x>y) ? (x):(y))
#define min2(x, y)     ((x<y) ? (x):(y))
#define IndexVect(i,j,k) ((k)*XDIM*YDIM + (j)*XDIM + (i))
#define PolyNum   15
#define DIFF_SCORE     4.4


typedef struct {
  float x;
  float y;
  float z;
}VECTOR;

typedef struct {
  float x1;
  float y1;
  float z1;
  float x2;
  float y2;
  float z2;
  float x3;
  float y3;
  float z3;
}EIGENVECT;

typedef struct {
  int x;
  int y;
  int z;
}INTVECT;

int XDIM,YDIM,ZDIM;
VECTOR* grad_vec;
float *dataset;
float t_low;
float helix_width;

int v_num;
INTVECT *triangle;
unsigned char *result;

void GVF_Cluster(int x, int y, int z);
void LocateMaximum(float*, float*, float*);
VECTOR GetMinAxisAndThickness(float,float,float,float*,float*,float*);
VECTOR GetMaxAxisAndThickness(float,float,float,float*,float*,float*);
EIGENVECT GetEigenVector(float,float,float);
float GetThickness(float, float, float,float, float, float);
void DrawLine(float, float, float, float, float, float, float);


void StructureTensor(int xd, int yd, int zd, VECTOR* gradvec,unsigned char* rlst,float *data, 
		     float* tlow, float helix_wd, float min_hratio, float max_hratio, 
		     float sheet_wd, float min_sratio, float max_sratio, int *HSeedNum)
{
  int i,j,k;
  int m,n;
  int x,y,z;
  int number,zero_flag;
  float tx,ty,tz;
  float thick1,thick2,thick3;
  float tmp;
  VECTOR EigenVector;
  EIGENVECT eigen_vect;
  float back_avg,threshold;
  float x00,x01,x10,x11,y0,y1;
  float min_helix_wd,max_helix_wd;
  float min_sheet_wd,max_sheet_wd;
  

  XDIM = xd;
  YDIM = yd;
  ZDIM = zd;
  grad_vec = gradvec;
  dataset = data;
  t_low = *tlow;
  result = rlst;
  helix_width = helix_wd;
  min_helix_wd = helix_wd*min_hratio;
  max_helix_wd = helix_wd*max_hratio;
  min_sheet_wd = sheet_wd*min_sratio;
  max_sheet_wd = sheet_wd*max_sratio;
  
  back_avg = (dataset[IndexVect(1,1,1)]+
	      dataset[IndexVect(XDIM-2,1,1)]+
	      dataset[IndexVect(1,YDIM-2,1)]+
	      dataset[IndexVect(XDIM-2,YDIM-2,1)]+
	      dataset[IndexVect(1,1,ZDIM-2)]+
	      dataset[IndexVect(XDIM-2,1,ZDIM-2)]+
	      dataset[IndexVect(1,YDIM-2,ZDIM-2)]+
	      dataset[IndexVect(XDIM-2,YDIM-2,ZDIM-2)])/8.0+10.0;
	  
  
  /* Find the critical points */
  m = 0;
  for (z=0; z<ZDIM; z++)
    for (y=0; y<YDIM; y++) 
      for (x=0; x<XDIM; x++) {
	if (dataset[IndexVect(x,y,z)] > back_avg) {
	  number = 1;
	  zero_flag = 1;
	  
	  for (k = max2(z-1,0); k <= min2(z+1,ZDIM-1); k++)
	    for (j = max2(y-1,0); j <= min2(y+1,YDIM-1); j++)
	      for (i = max2(x-1,0); i <= min2(x+1,XDIM-1); i++) {
		if (i != x || j != y || k != z) {
		  tmp = (i-x)*grad_vec[IndexVect(i,j,k)].x+
		    (j-y)*grad_vec[IndexVect(i,j,k)].y+
		    (k-z)*grad_vec[IndexVect(i,j,k)].z;
		  if (tmp != 0)
		    zero_flag = 0;
		  if (tmp < 0)
		    number = 0;
		}
	      }
	  
	  if (grad_vec[IndexVect(x,y,z)].x == 0 &&
	      grad_vec[IndexVect(x,y,z)].y == 0 &&
	      grad_vec[IndexVect(x,y,z)].z == 0 &&
	      zero_flag == 1)
	    result[IndexVect(x,y,z)] = 0;
	  else {
	    if (number == 1) {
	      m++;
	      result[IndexVect(x,y,z)] = 255;
	    }	  
	    else
	      result[IndexVect(x,y,z)] = 0;
	  }
	}
	else
	  result[IndexVect(x,y,z)] = 0;
      }
  printf("before clustering: %d \n",m);

  triangle = (INTVECT*)malloc(sizeof(INTVECT)*100);
  m = 0;
  for (z=0; z<ZDIM; z++)
    for (y=0; y<YDIM; y++) 
      for (x=0; x<XDIM; x++) {
        if (result[IndexVect(x,y,z)] == 255) {
	  result[IndexVect(x,y,z)] = 0;
          v_num = 0;
	  triangle[v_num].x = x;
	  triangle[v_num].y = y;
	  triangle[v_num].z = z;
	  v_num++;
	  GVF_Cluster(x, y, z);
	  
	  tmp = 0;
	  for (n=0; n<v_num; n++) {
	    if (dataset[IndexVect(triangle[n].x,triangle[n].y,triangle[n].z)] 
		> tmp) {
	      tmp = dataset[IndexVect(triangle[n].x,triangle[n].y,triangle[n].z)];
	      i = triangle[n].x;
	      j = triangle[n].y;
	      k = triangle[n].z;
	    }
	  }
	  
	  result[IndexVect(i,j,k)] = 254;
	  m++;
	}
      }
  printf("after clustering: %d \n",m);
 
  if (t_low < 0) {
    threshold = 0.0;
    number = 0;
    for (k=0; k<ZDIM; k++) 
      for (j=0; j<YDIM; j++) 
	for (i=0; i<XDIM; i++) {
	  if (result[IndexVect(i,j,k)] == 254) {
	    tx = (float)i;
	    ty = (float)j;
	    tz = (float)k;
	    eigen_vect = GetEigenVector(tx,ty,tz);
	    tx = (float)i + helix_width*eigen_vect.x1;
	    ty = (float)j + helix_width*eigen_vect.y1;
	    tz = (float)k + helix_width*eigen_vect.z1;
	    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
	    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
	    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
	    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
	    y0 = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
	    y1 = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
	    threshold += y0*((int)tz+1-tz) + y1*(tz-(int)tz);
	    number ++;

	    tx = (float)i - helix_width*eigen_vect.x1;
	    ty = (float)j - helix_width*eigen_vect.y1;
	    tz = (float)k - helix_width*eigen_vect.z1;
	    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
	    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
	    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
	    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
	    y0 = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
	    y1 = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
	    threshold += y0*((int)tz+1-tz) + y1*(tz-(int)tz);
	    number ++;
	  }
	}
    t_low = threshold/(float)(number);
    threshold = 0.0;
    number = 0;
    for (k=0; k<ZDIM; k++) 
      for (j=0; j<YDIM; j++) 
	for (i=0; i<XDIM; i++) {
	  if (result[IndexVect(i,j,k)] == 254) {
	    tx = (float)i;
	    ty = (float)j;
	    tz = (float)k;
	    eigen_vect = GetEigenVector(tx,ty,tz);
	    tx = (float)i + helix_width*eigen_vect.x1;
	    ty = (float)j + helix_width*eigen_vect.y1;
	    tz = (float)k + helix_width*eigen_vect.z1;
	    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
	    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
	    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
	    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
	    y0 = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
	    y1 = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
	    tmp = y0*((int)tz+1-tz) + y1*(tz-(int)tz);
	    if (tmp > t_low) {
	      threshold += tmp;
	      number ++;
	    }

	    tx = (float)i - helix_width*eigen_vect.x1;
	    ty = (float)j - helix_width*eigen_vect.y1;
	    tz = (float)k - helix_width*eigen_vect.z1;
	    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
	    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
	    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
	    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
	      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
	    y0 = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
	    y1 = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
	    tmp = y0*((int)tz+1-tz) + y1*(tz-(int)tz);
	    if (tmp > t_low) {
	      threshold += tmp;
	      number ++;
	    }
	  }
	}
    t_low = threshold/(float)(number);
    *tlow = t_low;
    printf("threshold: %f \n",t_low);
  }
  
  
  n = 0;
  for (k=0; k<ZDIM; k++) 
    for (j=0; j<YDIM; j++) 
      for (i=0; i<XDIM; i++) {
	
	if (result[IndexVect(i,j,k)] == 254) {
	 
	  tx = (float)i;
	  ty = (float)j;
	  tz = (float)k;
	  LocateMaximum(&tx,&ty,&tz);
	  
	  EigenVector = GetMinAxisAndThickness(tx,ty,tz,&thick1,&thick2,&thick3);
	  if ((thick1+thick2)*0.5 > min_helix_wd && 
	      (thick1+thick2)*0.5 < max_helix_wd && 
	      min2(thick1/thick2,thick2/thick1) > 
	      max2(thick1/thick3,thick2/thick3)) {
	    result[IndexVect(i,j,k)] = 111;
	    n++;
	  }
	  else {
	    EigenVector = GetMaxAxisAndThickness(tx,ty,tz,&thick1,&thick2,&thick3);
	    if (thick1 > min_sheet_wd && 
		thick1 < max_sheet_wd &&
		max2(thick1/thick2,thick1/thick3) < 
		min2(thick3/thick2,thick2/thick3)) {
	      result[IndexVect(i,j,k)] = 222;
	    }
	  }
	}
      }
 
  *HSeedNum = n;
}


VECTOR GetMinAxisAndThickness(float ax, float ay, float az, 
			      float* v1, float* v2, float* v3)
{
  EIGENVECT tmp;
  float p1,p2,p3;
  float tx,ty,tz;
  VECTOR temp;
  
  tmp = GetEigenVector(ax, ay, az);
  
  p1 = GetThickness(ax,ay,az,tmp.x1,tmp.y1,tmp.z1);
  p1 += GetThickness(ax,ay,az,-tmp.x1,-tmp.y1,-tmp.z1);
  p2 = GetThickness(ax,ay,az,tmp.x2,tmp.y2,tmp.z2);
  p2 += GetThickness(ax,ay,az,-tmp.x2,-tmp.y2,-tmp.z2);
  p3 = GetThickness(ax,ay,az,tmp.x3,tmp.y3,tmp.z3);
  p3 += GetThickness(ax,ay,az,-tmp.x3,-tmp.y3,-tmp.z3);
  
  tx = ax + helix_width*tmp.x3;
  ty = ay + helix_width*tmp.y3;
  tz = az + helix_width*tmp.z3;
  p1 += GetThickness(tx,ty,tz,tmp.x1,tmp.y1,tmp.z1);
  p1 += GetThickness(tx,ty,tz,-tmp.x1,-tmp.y1,-tmp.z1);
  p2 += GetThickness(tx,ty,tz,tmp.x2,tmp.y2,tmp.z2);
  p2 += GetThickness(tx,ty,tz,-tmp.x2,-tmp.y2,-tmp.z2);
  p3 += GetThickness(tx,ty,tz,tmp.x3,tmp.y3,tmp.z3);
  p3 += GetThickness(tx,ty,tz,-tmp.x3,-tmp.y3,-tmp.z3);
  
  tx = ax - helix_width*tmp.x3;
  ty = ay - helix_width*tmp.y3;
  tz = az - helix_width*tmp.z3;
  p1 += GetThickness(tx,ty,tz,tmp.x1,tmp.y1,tmp.z1);
  p1 += GetThickness(tx,ty,tz,-tmp.x1,-tmp.y1,-tmp.z1);
  p2 += GetThickness(tx,ty,tz,tmp.x2,tmp.y2,tmp.z2);
  p2 += GetThickness(tx,ty,tz,-tmp.x2,-tmp.y2,-tmp.z2);
  p3 += GetThickness(tx,ty,tz,tmp.x3,tmp.y3,tmp.z3);
  p3 += GetThickness(tx,ty,tz,-tmp.x3,-tmp.y3,-tmp.z3);
  
  *v1 = p1/6.0;
  *v2 = p2/6.0;
  *v3 = p3/6.0;
  

  temp.x = tmp.x3;
  temp.y = tmp.y3;
  temp.z = tmp.z3;
  return(temp);
}



VECTOR GetMaxAxisAndThickness(float ax, float ay, float az, 
			      float* t1, float* t2, float* t3)
{
  EIGENVECT tmp,tmpt;
  float p1,p2,p3;
  float tx,ty,tz;
  VECTOR temp;
  
  tmpt = GetEigenVector(ax, ay, az);
  temp.x = tmpt.x1;
  temp.y = tmpt.y1;
  temp.z = tmpt.z1;
  
  p1 = GetThickness(ax,ay,az,tmpt.x1,tmpt.y1,tmpt.z1);
  p1 += GetThickness(ax,ay,az,-tmpt.x1,-tmpt.y1,-tmpt.z1);
  p2 = GetThickness(ax,ay,az,tmpt.x2,tmpt.y2,tmpt.z2);
  p2 += GetThickness(ax,ay,az,-tmpt.x2,-tmpt.y2,-tmpt.z2);
  p3 = GetThickness(ax,ay,az,tmpt.x3,tmpt.y3,tmpt.z3);
  p3 += GetThickness(ax,ay,az,-tmpt.x3,-tmpt.y3,-tmpt.z3);

  tx = ax + helix_width*tmpt.x2;
  ty = ay + helix_width*tmpt.y2;
  tz = az + helix_width*tmpt.z2;
  tmp = GetEigenVector(tx, ty, tz);
  p1 += GetThickness(tx,ty,tz,tmp.x1,tmp.y1,tmp.z1);
  p1 += GetThickness(tx,ty,tz,-tmp.x1,-tmp.y1,-tmp.z1);
  p2 += GetThickness(tx,ty,tz,tmp.x2,tmp.y2,tmp.z2);
  p2 += GetThickness(tx,ty,tz,-tmp.x2,-tmp.y2,-tmp.z2);
  p3 += GetThickness(tx,ty,tz,tmp.x3,tmp.y3,tmp.z3);
  p3 += GetThickness(tx,ty,tz,-tmp.x3,-tmp.y3,-tmp.z3);

  tx = ax - helix_width*tmpt.x2;
  ty = ay - helix_width*tmpt.y2;
  tz = az - helix_width*tmpt.z2;
  tmp = GetEigenVector(tx, ty, tz);
  p1 += GetThickness(tx,ty,tz,tmp.x1,tmp.y1,tmp.z1);
  p1 += GetThickness(tx,ty,tz,-tmp.x1,-tmp.y1,-tmp.z1);
  p2 += GetThickness(tx,ty,tz,tmp.x2,tmp.y2,tmp.z2);
  p2 += GetThickness(tx,ty,tz,-tmp.x2,-tmp.y2,-tmp.z2);
  p3 += GetThickness(tx,ty,tz,tmp.x3,tmp.y3,tmp.z3);
  p3 += GetThickness(tx,ty,tz,-tmp.x3,-tmp.y3,-tmp.z3);

  tx = ax + helix_width*tmpt.x3;
  ty = ay + helix_width*tmpt.y3;
  tz = az + helix_width*tmpt.z3;
  tmp = GetEigenVector(tx, ty, tz);
  p1 += GetThickness(tx,ty,tz,tmp.x1,tmp.y1,tmp.z1);
  p1 += GetThickness(tx,ty,tz,-tmp.x1,-tmp.y1,-tmp.z1);
  p2 += GetThickness(tx,ty,tz,tmp.x2,tmp.y2,tmp.z2);
  p2 += GetThickness(tx,ty,tz,-tmp.x2,-tmp.y2,-tmp.z2);
  p3 += GetThickness(tx,ty,tz,tmp.x3,tmp.y3,tmp.z3);
  p3 += GetThickness(tx,ty,tz,-tmp.x3,-tmp.y3,-tmp.z3);

  tx = ax - helix_width*tmpt.x3;
  ty = ay - helix_width*tmpt.y3;
  tz = az - helix_width*tmpt.z3;
  tmp = GetEigenVector(tx, ty, tz);
  p1 += GetThickness(tx,ty,tz,tmp.x1,tmp.y1,tmp.z1);
  p1 += GetThickness(tx,ty,tz,-tmp.x1,-tmp.y1,-tmp.z1);
  p2 += GetThickness(tx,ty,tz,tmp.x2,tmp.y2,tmp.z2);
  p2 += GetThickness(tx,ty,tz,-tmp.x2,-tmp.y2,-tmp.z2);
  p3 += GetThickness(tx,ty,tz,tmp.x3,tmp.y3,tmp.z3);
  p3 += GetThickness(tx,ty,tz,-tmp.x3,-tmp.y3,-tmp.z3);

  *t1 = p1/10.0;
  *t2 = p2/10.0;
  *t3 = p3/10.0;
  
  return(temp);
}


EIGENVECT GetEigenVector(float ax, float ay, float az)
{
  int x,y,z;
  int nx,ny,nz;
  double x1, x2, x3;
  double a,b,Q;
  double c0,c1,c2;
  double A[3][3];
  double B[6];
  double theta, p;
  double tx,ty,tz;
  EIGENVECT tmp;
  
  
  nx = (int)(ax);
  ny = (int)(ay);
  nz = (int)(az);
  
  A[0][0] = 0;
  A[0][1] = 0;
  A[0][2] = 0;
  A[1][1] = 0;
  A[1][2] = 0;
  A[2][2] = 0;
  for (z=max2(0,nz-WINDOW+1); z<=min2(ZDIM-1,nz+WINDOW); z++) 
    for (y=max2(0,ny-WINDOW+1); y<=min2(YDIM-1,ny+WINDOW); y++) 
      for (x=max2(0,nx-WINDOW+1); x<=min2(XDIM-1,nx+WINDOW); x++) {
	p = (x-ax)*(x-ax)+(y-ay)*(y-ay)+(z-az)*(z-az);
	
	A[0][0] += exp(-p/(SIGMA*SIGMA))*grad_vec[IndexVect(x,y,z)].x
	  *grad_vec[IndexVect(x,y,z)].x;
	A[0][1] += exp(-p/(SIGMA*SIGMA))*grad_vec[IndexVect(x,y,z)].x
	  *grad_vec[IndexVect(x,y,z)].y;
	A[0][2] += exp(-p/(SIGMA*SIGMA))*grad_vec[IndexVect(x,y,z)].x
	  *grad_vec[IndexVect(x,y,z)].z;
	A[1][1] += exp(-p/(SIGMA*SIGMA))*grad_vec[IndexVect(x,y,z)].y
	  *grad_vec[IndexVect(x,y,z)].y;
	A[1][2] += exp(-p/(SIGMA*SIGMA))*grad_vec[IndexVect(x,y,z)].y
	  *grad_vec[IndexVect(x,y,z)].z;
	A[2][2] += exp(-p/(SIGMA*SIGMA))*grad_vec[IndexVect(x,y,z)].z
	  *grad_vec[IndexVect(x,y,z)].z;
      }
  
  
  A[1][0] = A[0][1];
  A[2][0] = A[0][2];
  A[2][1] = A[1][2];
  
  c0 = A[0][0]*A[1][1]*A[2][2]+2*A[0][1]*A[0][2]*A[1][2]-A[0][0]*A[1][2]*A[1][2]
    -A[1][1]*A[0][2]*A[0][2]-A[2][2]*A[0][1]*A[0][1];
  c1 = A[0][0]*A[1][1]-A[0][1]*A[0][1]+A[0][0]*A[2][2]-
    A[0][2]*A[0][2]+A[1][1]*A[2][2]-A[1][2]*A[1][2];
  c2 = A[0][0]+A[1][1]+A[2][2];
  
  a = (3.0*c1-c2*c2)/3.0;
  b = (-2.0*c2*c2*c2+9.0*c1*c2-27.0*c0)/27.0;
  Q = b*b/4.0+a*a*a/27.0;
  	    
  theta = atan2(sqrt(-Q),-0.5*b);
  p = sqrt(0.25*b*b-Q);
  x1 = c2/3.0+2.0*pow(p,1.0/3.0)*cos(theta/3.0);
  x2 = c2/3.0-pow(p,1.0/3.0)*(cos(theta/3.0)+sqrt(3.0)*sin(theta/3.0));
  x3 = c2/3.0-pow(p,1.0/3.0)*(cos(theta/3.0)-sqrt(3.0)*sin(theta/3.0));

  tx = max2(x1,max2(x2,x3));
  if (tx == x1) {
    if (x2 >= x3) {
      ty = x2;
      tz = x3;
    }
    else {
      ty = x3;
      tz = x2;
    }
  }
  else if (tx == x2) {
    if (x1 >= x3) {
      ty = x1;
      tz = x3;
    }
    else {
      ty = x3;
      tz = x1;
    }
  }
  else if (tx == x3) {
    if (x1 >= x2) {
      ty = x1;
      tz = x2;
    }
    else {
      ty = x2;
      tz = x1;
    }
  }
  x1 = tx;
  x2 = ty;
  x3 = tz;
  
  A[0][0] -= x1;
  A[1][1] -= x1;
  A[2][2] -= x1;
  B[0] = A[1][1]*A[2][2]-A[1][2]*A[1][2];
  B[1] = A[0][2]*A[1][2]-A[0][1]*A[2][2];
  B[2] = A[0][0]*A[2][2]-A[0][2]*A[0][2];
  B[3] = A[0][1]*A[1][2]-A[0][2]*A[1][1];
  B[4] = A[0][1]*A[0][2]-A[1][2]*A[0][0];
  B[5] = A[0][0]*A[1][1]-A[0][1]*A[0][1];
  c0 = B[0]*B[0]+B[1]*B[1]+B[3]*B[3];
  c1 = B[1]*B[1]+B[2]*B[2]+B[4]*B[4];
  c2 = B[3]*B[3]+B[4]*B[4]+B[5]*B[5];
  if (c0 >= c1 && c0 >= c2) {
    tx = B[0];
    ty = B[1];
    tz = B[3];
  }
  else if (c1 >= c0 && c1 >= c2) {
    tx = B[1];
    ty = B[2];
    tz = B[4];
  }
  else if (c2 >= c0 && c2 >= c1) {
    tx = B[3];
    ty = B[4];
    tz = B[5];
  }
  p = sqrt(tx*tx+ty*ty+tz*tz);
  tx /= p;
  ty /= p;
  tz /= p;
  tmp.x1 = tx;
  tmp.y1 = ty;
  tmp.z1 = tz;
  A[0][0] += x1;
  A[1][1] += x1;
  A[2][2] += x1;
  
 
  A[0][0] -= x2;
  A[1][1] -= x2;
  A[2][2] -= x2;
  B[0] = A[1][1]*A[2][2]-A[1][2]*A[1][2];
  B[1] = A[0][2]*A[1][2]-A[0][1]*A[2][2];
  B[2] = A[0][0]*A[2][2]-A[0][2]*A[0][2];
  B[3] = A[0][1]*A[1][2]-A[0][2]*A[1][1];
  B[4] = A[0][1]*A[0][2]-A[1][2]*A[0][0];
  B[5] = A[0][0]*A[1][1]-A[0][1]*A[0][1];
  c0 = B[0]*B[0]+B[1]*B[1]+B[3]*B[3];
  c1 = B[1]*B[1]+B[2]*B[2]+B[4]*B[4];
  c2 = B[3]*B[3]+B[4]*B[4]+B[5]*B[5];
  if (c0 >= c1 && c0 >= c2) {
    tx = B[0];
    ty = B[1];
    tz = B[3];
  }
  else if (c1 >= c0 && c1 >= c2) {
    tx = B[1];
    ty = B[2];
    tz = B[4];
  }
  else if (c2 >= c0 && c2 >= c1) {
    tx = B[3];
    ty = B[4];
    tz = B[5];
  }
  p = sqrt(tx*tx+ty*ty+tz*tz);
  tx /= p;
  ty /= p;
  tz /= p;
  tmp.x2 = tx;
  tmp.y2 = ty;
  tmp.z2 = tz;
  
  tmp.x3 = tmp.y1*tz-tmp.z1*ty;
  tmp.y3 = tmp.z1*tx-tmp.x1*tz;
  tmp.z3 = tmp.x1*ty-tmp.y1*tx;

  return(tmp);
}



void GVF_Cluster(int x, int y, int z)
{
  int m, n, l;

  for (l = max2(z-1,0); l <= min2(z+1,ZDIM-1); l++)
    for (n = max2(y-1,0); n <= min2(y+1,YDIM-1); n++)
      for (m = max2(x-1,0); m <= min2(x+1,XDIM-1); m++) {
	if (result[IndexVect(m,n,l)] == 255) {
	  result[IndexVect(m,n,l)] = 0;
	  triangle[v_num].x = m;
	  triangle[v_num].y = n;
	  triangle[v_num].z = l;
	  v_num++;
	  
	  GVF_Cluster(m, n, l);
	}
      }
  
}



void LocateMaximum(float* ax, float* ay, float* az)
{
  int i,j,k;
  float a,b,c;
  float dx,dy,dz;
  float t;
  
  
  i = (int)(*ax+0.5);
  j = (int)(*ay+0.5);
  k = (int)(*az+0.5);
  
  a = dataset[IndexVect(max2(0,i-1),j,k)];
  b = dataset[IndexVect(i,j,k)];
  c = dataset[IndexVect(min2(XDIM-1,i+1),j,k)];
  if (a >= b && a >= c) 
    dx = (float)i-0.5;
  else if (c >= b && c >= a)
    dx = (float)i+0.5;
  else {
    if (a >= c) {
      t = 0.5*(a-c)/(b-c);
      dx = (float)i-t;
    }
    else {
      t = 0.5*(c-a)/(b-a);
      dx = (float)i+t;
    }
  }

  a = dataset[IndexVect(i,max2(0,j-1),k)];
  b = dataset[IndexVect(i,j,k)];
  c = dataset[IndexVect(i,min2(YDIM-1,j+1),k)];
  if (a >= b && a >= c) 
    dy = (float)j-0.5;
  else if (c >= b && c >= a)
    dy = (float)j+0.5;
  else {
    if (a >= c) {
      t = 0.5*(a-c)/(b-c);
      dy = (float)j-t;
    }
    else {
      t = 0.5*(c-a)/(b-a);
      dy = (float)j+t;
    }
  }

  a = dataset[IndexVect(i,j,max2(0,k-1))];
  b = dataset[IndexVect(i,j,k)];
  c = dataset[IndexVect(i,j,min2(ZDIM-1,k+1))];
  if (a >= b && a >= c) 
    dz = (float)k-0.5;
  else if (c >= b && c >= a)
    dz = (float)k+0.5;
  else {
    if (a >= c) {
      t = 0.5*(a-c)/(b-c);
      dz = (float)k-t;
    }
    else {
      t = 0.5*(c-a)/(b-a);
      dz = (float)k+t;
    }
  }

  *ax = dx;
  *ay = dy;
  *az = dz;
 
}




float GetThickness(float ax, float ay, float az,
		    float tx, float ty, float tz)
{
  float dx,dy,dz;
  float thick, score;
  float x00,x01,x10,x11,y0,y1;


  dx = ax;
  dy = ay;
  dz = az;
  if (dx<0 || dx>XDIM-1 || dy<0 || dy>YDIM-1 || dz<0 || dz>ZDIM-1) 
    return(0);

  x00 = dataset[IndexVect((int)dx,(int)dy,(int)dz)]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),(int)dy,(int)dz)]*(dx-(int)dx);
  x01 = dataset[IndexVect((int)dx,(int)dy,min2(ZDIM-1,(int)dz+1))]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),(int)dy,min2(ZDIM-1,(int)dz+1))]*(dx-(int)dx);
  x10 = dataset[IndexVect((int)dx,min2(YDIM-1,(int)dy+1),(int)dz)]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),min2(YDIM-1,(int)dy+1),(int)dz)]*(dx-(int)dx);
  x11 = dataset[IndexVect((int)dx,min2(YDIM-1,(int)dy+1),min2(ZDIM-1,(int)dz+1))]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),min2(YDIM-1,(int)dy+1),min2(ZDIM-1,(int)dz+1))]*(dx-(int)dx);
  y0  = x00*((int)dy+1-dy) + x10*(dy-(int)dy);
  y1  = x01*((int)dy+1-dy) + x11*(dy-(int)dy);
  score = y0*((int)dz+1-dz) + y1*(dz-(int)dz);
    
  thick = 0;
  while (score > t_low) {
    thick += 0.5;
    dx = ax + tx*thick;
    dy = ay + ty*thick;
    dz = az + tz*thick;
    if (dx<0 || dx>XDIM-1 || dy<0 || dy>YDIM-1 || dz<0 || dz>ZDIM-1)
      break;
    x00 = dataset[IndexVect((int)dx,(int)dy,(int)dz)]*((int)dx+1-dx)+
      dataset[IndexVect(min2(XDIM-1,(int)dx+1),(int)dy,(int)dz)]*(dx-(int)dx);
    x01 = dataset[IndexVect((int)dx,(int)dy,min2(ZDIM-1,(int)dz+1))]*((int)dx+1-dx)+
      dataset[IndexVect(min2(XDIM-1,(int)dx+1),(int)dy,min2(ZDIM-1,(int)dz+1))]*(dx-(int)dx);
    x10 = dataset[IndexVect((int)dx,min2(YDIM-1,(int)dy+1),(int)dz)]*((int)dx+1-dx)+
      dataset[IndexVect(min2(XDIM-1,(int)dx+1),min2(YDIM-1,(int)dy+1),(int)dz)]*(dx-(int)dx);
    x11 = dataset[IndexVect((int)dx,min2(YDIM-1,(int)dy+1),min2(ZDIM-1,(int)dz+1))]*((int)dx+1-dx)+
      dataset[IndexVect(min2(XDIM-1,(int)dx+1),min2(YDIM-1,(int)dy+1),min2(ZDIM-1,(int)dz+1))]*(dx-(int)dx);
    y0  = x00*((int)dy+1-dy) + x10*(dy-(int)dy);
    y1  = x01*((int)dy+1-dy) + x11*(dy-(int)dy);
    score = y0*((int)dz+1-dz) + y1*(dz-(int)dz);
  }

  thick -= 0.25;
  dx = ax + tx*thick;
  dy = ay + ty*thick;
  dz = az + tz*thick;
  x00 = dataset[IndexVect((int)dx,(int)dy,(int)dz)]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),(int)dy,(int)dz)]*(dx-(int)dx);
  x01 = dataset[IndexVect((int)dx,(int)dy,min2(ZDIM-1,(int)dz+1))]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),(int)dy,min2(ZDIM-1,(int)dz+1))]*(dx-(int)dx);
  x10 = dataset[IndexVect((int)dx,min2(YDIM-1,(int)dy+1),(int)dz)]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),min2(YDIM-1,(int)dy+1),(int)dz)]*(dx-(int)dx);
  x11 = dataset[IndexVect((int)dx,min2(YDIM-1,(int)dy+1),min2(ZDIM-1,(int)dz+1))]*((int)dx+1-dx)+
    dataset[IndexVect(min2(XDIM-1,(int)dx+1),min2(YDIM-1,(int)dy+1),min2(ZDIM-1,(int)dz+1))]*(dx-(int)dx);
  y0  = x00*((int)dy+1-dy) + x10*(dy-(int)dy);
  y1  = x01*((int)dy+1-dy) + x11*(dy-(int)dy);
  score = y0*((int)dz+1-dz) + y1*(dz-(int)dz);
  
  if (score > t_low)
    thick += 0.125;
  else
    thick -= 0.125;
  
  return(thick);
}


