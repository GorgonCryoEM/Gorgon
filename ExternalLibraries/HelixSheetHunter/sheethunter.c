/**************************************
 * Zeyun Yu (zeyun@cs.utexas.edu)    *
 * Department of Computer Science    *
 * University of Texas at Austin     *
 **************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>  
#include <memory.h>
#include "march.h"

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

typedef struct {
  unsigned short x;
  unsigned short y;
  unsigned short z;
  unsigned short parent;
  float dx;
  float dy;
  float dz;
}ARRAY;

typedef struct PNTSIndex PNTS;
struct PNTSIndex{
  float x;  
  float y;  
  float z; 
  PNTS *next;
};

typedef struct {
  float dx;
  float dy;
  float dz;
  float thickness;
  int size;
  PNTS *points;
}SEEDS;



#define PolyNum   30
#define max2(x, y) ((x>y) ? (x):(y))
#define min2(x, y) ((x<y) ? (x):(y))
#define PIE           3.1415926
#define WINDOW 3
#define IndexVect(i,j,k) ((k)*XDIM*YDIM + (j)*XDIM + (i))


SEEDS *AllSheet;
int SheetSeedNum;
float *SheetDistMatrix;

int v_num, t_num;
VECTOR *vertex;
INTVECT *triangle;
unsigned short *tri_index;

INTVECT *mc_edge;
char *mc_visited;
unsigned char *mc_sign;
ARRAY *stack;
  
float t_low;
float min_sheet_wd,max_sheet_wd;
int XDIM, YDIM, ZDIM;
float *dataset;
VECTOR* grad_vec;
float helix_draw;

void DrawLine(float, float, float, float, float, float, float);
void DrawLine2(float, float, float, float, float, float, VECTOR*,VECTOR*);
void LocateMaximum(float*, float*, float*);
VECTOR GetCenter(int, int, int);
VECTOR GetMaxAxisAndThickness(float,float,float,float*,float*,float*);
void TraceSheet(int);
float GetThickness(float, float, float,float, float, float);
EIGENVECT GetEigenVector(float, float, float);


void SheetHunter(int xd,int yd,int zd,float *span,float *orig,float *data,unsigned char *seed,VECTOR* gradvec,
     float tlow,float helixwd,float min_hratio, float max_hratio,float hlength,float sheetwd,float min_sratio,
     float max_sratio,float sheetextend,FILE *fp2,SEEDS *All_Helix,int HSeedNum,FILE *fp1,FILE *fp3)
{
  int i,j,k;
  int i0,j0,k0;
  int num,n,m;
  VECTOR EigenVector;
  EIGENVECT AllEigenVect;
  PNTS *pnt_obj,*pnt_prv,*pnt_start;
  float tmpx,tmpy,tmpz;
  float thick1,thick2,thick3;
  int beta_num,flag;
  int v_num0,t_num0;
  float min_mx,min_my,min_mz;
  float min_nx,min_ny,min_nz;
  float tempx,tempy,tempz;
  float distance,dist;
  float min_helix_wd,max_helix_wd;
  float start_x,start_y,start_z;
  float end_x,end_y,end_z;
  float template,angle;
  VECTOR *helix_pnts1,*helix_pnts2;
  float x,y,z;
  float xx,yy,zz;
  float a[3][3],b[3][3];
  float theta, phi;
  SEEDS *AllHelix_list; 
  
  
  helix_draw = 0.8*helixwd;
  XDIM = xd;
  YDIM = yd;
  ZDIM = zd;
  dataset = data;
  grad_vec = gradvec;
  t_low = tlow;
  min_sheet_wd = sheetwd*min_sratio/sheetextend;
  max_sheet_wd = sheetwd*max_sratio*sheetextend;
  min_helix_wd = helixwd*(min_hratio/2.0+0.5);
  max_helix_wd = helixwd*(max_hratio/2.0+0.5);
  
  vertex = (VECTOR*)malloc(sizeof(VECTOR)*50000*PolyNum);
  triangle = (INTVECT*)malloc(sizeof(INTVECT)*50000*PolyNum);
  tri_index = (unsigned short*)malloc(sizeof(unsigned short)*50000*PolyNum);
  v_num = 0;
  t_num = 0;

  mc_edge = (INTVECT*)malloc(sizeof(INTVECT)*XDIM*YDIM*ZDIM);
  mc_visited = (char*)malloc(sizeof(char)*XDIM*YDIM*ZDIM);
  mc_sign = (unsigned char*)malloc(sizeof(unsigned char)*XDIM*YDIM*ZDIM);
  stack = (ARRAY*)malloc(sizeof(ARRAY)*XDIM*YDIM*ZDIM/5);
  for (k=0; k<ZDIM; k++)
    for (j=0; j<YDIM; j++) 
      for (i=0; i<XDIM; i++) {
	mc_edge[IndexVect(i,j,k)].x = -1;
	mc_edge[IndexVect(i,j,k)].y = -1;
	mc_edge[IndexVect(i,j,k)].z = -1;
	mc_visited[IndexVect(i,j,k)] = 0;
	mc_sign[IndexVect(i,j,k)] = 0;
      }

  
  SheetSeedNum = 0;
  for (k=0; k<ZDIM; k++)
    for (j=0; j<YDIM; j++) 
      for (i=0; i<XDIM; i++) {
	if (seed[IndexVect(i,j,k)] == 222) 
	  SheetSeedNum++;
      }
  AllSheet = (SEEDS*)malloc(sizeof(SEEDS)*SheetSeedNum);
  SheetDistMatrix = (float*)malloc(sizeof(float)*SheetSeedNum*SheetSeedNum);
  for (j = 0; j < SheetSeedNum; j++) 
    for (i = 0; i < SheetSeedNum; i++)
      SheetDistMatrix[j*SheetSeedNum+i] = 9999999.0f;
  

  beta_num = 0;
  for (k=0; k<ZDIM; k++)
    for (j=0; j<YDIM; j++) 
      for (i=0; i<XDIM; i++) {
	if (seed[IndexVect(i,j,k)] == 222) {
	  seed[IndexVect(i,j,k)] = 0;
	  tmpx = (float)i;
	  tmpy = (float)j;
	  tmpz = (float)k;
	  LocateMaximum(&tmpx,&tmpy,&tmpz);
	  EigenVector = GetMaxAxisAndThickness(tmpx,tmpy,tmpz,&thick1,&thick2,&thick3);
	  /*
	  DrawLine(tmpx-0.3*EigenVector.x,tmpy-0.3*EigenVector.y,tmpz-0.3*EigenVector.z,
	           tmpx+0.3*EigenVector.x,tmpy+0.3*EigenVector.y,tmpz+0.5\3*EigenVector.z,1);
	  */
	  if (mc_visited[IndexVect((int)tmpx,(int)tmpy,(int)tmpz)] == 0) {
	    mc_visited[IndexVect((int)tmpx,(int)tmpy,(int)tmpz)] = 1;
	    
	    AllSheet[beta_num].dx = EigenVector.x;
	    AllSheet[beta_num].dy = EigenVector.y;
	    AllSheet[beta_num].dz = EigenVector.z;
	    AllSheet[beta_num].thickness = thick1;
	    AllSheet[beta_num].size = 1;
	    pnt_obj = (PNTS*)malloc(sizeof(PNTS));
	    pnt_obj->x = tmpx;
	    pnt_obj->y = tmpy;
	    pnt_obj->z = tmpz;
	    pnt_obj->next = NULL;
	    AllSheet[beta_num].points = pnt_obj;
	    v_num0 = v_num;
	    t_num0 = t_num;
	    TraceSheet(beta_num);
	    
	    beta_num++;
	  }
	}
	
      }
  printf("beta_num = %d \n",beta_num);
  

  /* Old
  for (num = 0; num < HSeedNum; num++) {
    if (AllHelix_list[num].points == NULL)
      continue;
    else {
      pnt_obj = AllHelix_list[num].points;
      i = (int)(pnt_obj->x);
      j = (int)(pnt_obj->y);
      k = (int)(pnt_obj->z);
      i0 = i;
      j0 = j;
      k0 = k;
      if (mc_visited[IndexVect(i,j,k)] == 0) {
	m = 1;
	n = 0;
      }
      else {
	m = 0;
	n = 1;
      }
      while (pnt_obj->next != NULL) {
	pnt_obj = pnt_obj->next;
	i = (int)(pnt_obj->x);
	j = (int)(pnt_obj->y);
	k = (int)(pnt_obj->z);
	if (i!=i0 || j!=j0 || k!=k0) {
	  if (mc_visited[IndexVect(i,j,k)] == 0) 
	    m++;
	  else
	    n++;
	  i0 = i;
	  j0 = j;
	  k0 = k;
	}
      }
      printf("%d %d \n",m,n);
      if (m <= n) {
	AllHelix_list[num].points = NULL;
	AllHelix_list[num].size = 0;
      }
    }
  }
  */

  AllHelix_list = (SEEDS*)malloc(sizeof(SEEDS)*5*HSeedNum);
  for (num = 0; num < HSeedNum; num++) {
    AllHelix_list[num].dx = All_Helix[num].dx;
    AllHelix_list[num].dy = All_Helix[num].dy;
    AllHelix_list[num].dz = All_Helix[num].dz;
    AllHelix_list[num].thickness = All_Helix[num].thickness;
    AllHelix_list[num].size = All_Helix[num].size;
    AllHelix_list[num].points = All_Helix[num].points;
  }
  m = HSeedNum;
  for (num = 0; num < HSeedNum; num++) {
    if (AllHelix_list[num].points == NULL)
      continue;
    else {
      pnt_obj = AllHelix_list[num].points;
      pnt_prv = pnt_obj;
      n = 0;
      while (pnt_obj->next != NULL) {
	pnt_obj = pnt_obj->next;
	i = (int)(0.5*(pnt_obj->x+pnt_prv->x));
	j = (int)(0.5*(pnt_obj->y+pnt_prv->y));
	k = (int)(0.5*(pnt_obj->z+pnt_prv->z));
	flag = 0;
	for (k0=max2(0,k-1); k0<=min2(ZDIM-1,k+1); k0++) 
	  for (j0=max2(0,j-1); j0<=min2(YDIM-1,j+1); j0++) 
	    for (i0=max2(0,i-1); i0<=min2(XDIM-1,i+1); i0++) {
	      if (mc_visited[IndexVect(i0,j0,k0)] > 0)
		flag = 1;
	    }
	if (flag == 1) {
	  if (n == 1) {
	    pnt_prv->next = NULL;
	    AllHelix_list[m].points = pnt_start;
	    m++;
	    n = 0;
	  }
	}
	else {
	  if (n == 0) {
	    pnt_start = pnt_prv;
	    n = 1;
	  }
	}
	pnt_prv = pnt_obj;
      }
      if (n == 1) {
	pnt_prv->next = NULL;
	AllHelix_list[m].points = pnt_start;
	m++;
      }

      AllHelix_list[num].points = NULL;
      AllHelix_list[num].size = 0;
    }
  }
  HSeedNum = m;

  for (n = 0; n < HSeedNum; n++) {
    pnt_obj = AllHelix_list[n].points;
    distance = 0.0;
    while (pnt_obj != NULL && pnt_obj->next != NULL) {	
      tmpx = pnt_obj->x;
      tmpy = pnt_obj->y;
      tmpz = pnt_obj->z;
      pnt_obj = pnt_obj->next;
      tempx = pnt_obj->x;
      tempy = pnt_obj->y;
      tempz = pnt_obj->z;
      distance += sqrt((tmpx-tempx)*(tmpx-tempx)+(tmpy-tempy)*(tmpy-tempy)+
		       (tmpz-tempz)*(tmpz-tempz));
    }
    	  
    if (distance < 0.5*hlength) {
      AllHelix_list[n].points = NULL;
      AllHelix_list[n].size = 0;
    }
  }
  
  
  fprintf(fp2, "#VRML V2.0 utf8\n");
  fprintf(fp2, "Shape {\n");
  fprintf(fp2, "        appearance Appearance {\n");
  fprintf(fp2, "                material Material {\n");
  fprintf(fp2, "                        ambientIntensity 0.8\n");
  fprintf(fp2, "                        diffuseColor 0.99 0.0 0.7\n");
  fprintf(fp2, "                        specularColor	0.8 0.8 0.01\n");
  fprintf(fp2, "                        emissiveColor	0.0 0.01 0.01\n");
  fprintf(fp2, "                        shininess	0.5\n");
  fprintf(fp2, "                        transparency	0\n");
  fprintf(fp2, "                }\n");
  fprintf(fp2, "        }\n");
  fprintf(fp2, "        geometry IndexedFaceSet {\n");
  fprintf(fp2, "                creaseAngle 1.0\n");
  fprintf(fp2, "                coord Coordinate {\n");
  fprintf(fp2, "                        point [\n");
  for (num = 0; num < v_num; num++) 
    fprintf(fp2, "%f %f %f\n",vertex[num].x*span[0]+orig[0],
	    vertex[num].y*span[1]+orig[1],vertex[num].z*span[2]+orig[2]);
  fprintf(fp2, "			]\n");
  fprintf(fp2, "		}\n");
  fprintf(fp2, "		coordIndex [\n");
  for (num = 0; num < t_num; num++) {
    if (AllSheet[tri_index[num]].size >= 5 && 
	AllSheet[tri_index[num]].thickness < 0.5*(max_sheet_wd+sheetwd)) {
      fprintf(fp2, "%d %d %d -1\n",triangle[num].x,triangle[num].y,triangle[num].z);
      fprintf(fp2, "%d %d %d -1\n",triangle[num].x,triangle[num].z,triangle[num].y);
    }
  }
  fprintf(fp2, "		]\n");
  fprintf(fp2, "	}\n");
  fprintf(fp2, "}\n");
  fclose(fp2);
  

  v_num = 0;
  t_num = 0;
  for (num = 0; num < HSeedNum; num++) {
    if (AllHelix_list[num].points == NULL)
      continue;
    else {
      pnt_obj = AllHelix_list[num].points;
      min_mx = pnt_obj->x;
      min_my = pnt_obj->y;
      min_mz = pnt_obj->z;
      tmpx = min_mx;
      tmpy = min_my;
      tmpz = min_mz;
      n = 1;
      start_x = min_mx;
      start_y = min_my;
      start_z = min_mz;
      template = helixwd;
      AllEigenVect = GetEigenVector(start_x, start_y, start_z);
      tempx = AllEigenVect.x3;
      tempy = AllEigenVect.y3;
      tempz = AllEigenVect.z3;
      thick1 = GetThickness(start_x, start_y, start_z,AllEigenVect.x1,AllEigenVect.y1,AllEigenVect.z1);
      thick1 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x1,-AllEigenVect.y1,-AllEigenVect.z1);
      thick2 = GetThickness(start_x, start_y, start_z,AllEigenVect.x2,AllEigenVect.y2,AllEigenVect.z2);
      thick2 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x2,-AllEigenVect.y2,-AllEigenVect.z2);
      thick3 = GetThickness(start_x, start_y, start_z,AllEigenVect.x3,AllEigenVect.y3,AllEigenVect.z3);
      thick3 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x3,-AllEigenVect.y3,-AllEigenVect.z3);
      pnt_obj = pnt_obj->next;
      end_x = pnt_obj->x;
      end_y = pnt_obj->y;
      end_z = pnt_obj->z;
      while (1) {
	distance = sqrt((start_x-end_x)*(start_x-end_x)+(start_y-end_y)*(start_y-end_y)+
			(start_z-end_z)*(start_z-end_z));
	if (template <= distance) {
	  start_x = (end_x-start_x)*template/distance+start_x;
	  start_y = (end_y-start_y)*template/distance+start_y;
	  start_z = (end_z-start_z)*template/distance+start_z;
	  tmpx += start_x;
	  tmpy += start_y;
	  tmpz += start_z;
	  n++;
	  template = helixwd;
	  AllEigenVect = GetEigenVector(start_x, start_y, start_z);
	  if (tempx*AllEigenVect.x3+tempy*AllEigenVect.y3+tempz*AllEigenVect.z3 <= 0) {
	    tempx -= AllEigenVect.x3;
	    tempy -= AllEigenVect.y3;
	    tempz -= AllEigenVect.z3;
	  }
	  else {
	    tempx += AllEigenVect.x3;
	    tempy += AllEigenVect.y3;
	    tempz += AllEigenVect.z3;
	  }
	  thick1 += GetThickness(start_x, start_y, start_z,AllEigenVect.x1,AllEigenVect.y1,AllEigenVect.z1);
	  thick1 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x1,-AllEigenVect.y1,-AllEigenVect.z1);
	  thick2 += GetThickness(start_x, start_y, start_z,AllEigenVect.x2,AllEigenVect.y2,AllEigenVect.z2);
	  thick2 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x2,-AllEigenVect.y2,-AllEigenVect.z2);
	  thick3 += GetThickness(start_x, start_y, start_z,AllEigenVect.x3,AllEigenVect.y3,AllEigenVect.z3);
	  thick3 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x3,-AllEigenVect.y3,-AllEigenVect.z3);
	}
	else {
	  template -= distance;
	  start_x = end_x;
	  start_y = end_y;
	  start_z = end_z;
	  pnt_obj = pnt_obj->next;
	  if (pnt_obj == NULL) 
	    break;
	  end_x = pnt_obj->x;
	  end_y = pnt_obj->y;
	  end_z = pnt_obj->z;
	}
      }
      
      min_nx = start_x;
      min_ny = start_y;
      min_nz = start_z;
      tmpx += start_x;
      tmpy += start_y;
      tmpz += start_z;
      n++;
      tmpx /= (float)(n);
      tmpy /= (float)(n);
      tmpz /= (float)(n);
      AllEigenVect = GetEigenVector(start_x, start_y, start_z);
      if (tempx*AllEigenVect.x3+tempy*AllEigenVect.y3+tempz*AllEigenVect.z3 <= 0) {
	tempx -= AllEigenVect.x3;
	tempy -= AllEigenVect.y3;
	tempz -= AllEigenVect.z3;
      }
      else {
	tempx += AllEigenVect.x3;
	tempy += AllEigenVect.y3;
	tempz += AllEigenVect.z3;
      }
      thick1 += GetThickness(start_x, start_y, start_z,AllEigenVect.x1,AllEigenVect.y1,AllEigenVect.z1);
      thick1 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x1,-AllEigenVect.y1,-AllEigenVect.z1);
      thick2 += GetThickness(start_x, start_y, start_z,AllEigenVect.x2,AllEigenVect.y2,AllEigenVect.z2);
      thick2 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x2,-AllEigenVect.y2,-AllEigenVect.z2);
      thick3 += GetThickness(start_x, start_y, start_z,AllEigenVect.x3,AllEigenVect.y3,AllEigenVect.z3);
      thick3 += GetThickness(start_x, start_y, start_z,-AllEigenVect.x3,-AllEigenVect.y3,-AllEigenVect.z3);
      thick1 = 0.5*thick1/(float)(n);
      thick2 = 0.5*thick2/(float)(n);
      thick3 = 0.5*thick3/(float)(n);
      distance = sqrt(tempx*tempx+tempy*tempy+tempz*tempz);
      tempx /= distance;
      tempy /= distance;
      tempz /= distance;
      
      if ((thick1+thick2)*0.5 > min_helix_wd && (thick1+thick2)*0.5 < max_helix_wd && 
	  min2(thick1/thick2,thick2/thick1) > max2(thick1/thick3,thick2/thick3)) {
	distance = (min_mx-tmpx)*tempx+(min_my-tmpy)*tempy+(min_mz-tmpz)*tempz; 
	min_mx = tmpx + distance*tempx;
	min_my = tmpy + distance*tempy;
	min_mz = tmpz + distance*tempz;
	distance = (min_nx-tmpx)*tempx+(min_ny-tmpy)*tempy+(min_nz-tmpz)*tempz; 
	min_nx = tmpx + distance*tempx;
	min_ny = tmpy + distance*tempy;
	min_nz = tmpz + distance*tempz;
	DrawLine(min_mx,min_my,min_mz,min_nx,min_ny,min_nz,helix_draw);
      }
      else {
	AllHelix_list[num].points = NULL;
	AllHelix_list[num].size = 0;
      }
    }
  }


  fprintf(fp1, "#VRML V2.0 utf8\n");
  fprintf(fp1, "Shape {\n");
  fprintf(fp1, "        appearance Appearance {\n");
  fprintf(fp1, "                material Material {\n");
  fprintf(fp1, "                        ambientIntensity 0.8\n");
  fprintf(fp1, "                        diffuseColor 0.0 0.99 0.7\n");
  fprintf(fp1, "                        specularColor	0.8 0.8 0.01\n");
  fprintf(fp1, "                        emissiveColor	0.0 0.01 0.01\n");
  fprintf(fp1, "                        shininess	0.5\n");
  fprintf(fp1, "                        transparency	0\n");
  fprintf(fp1, "                }\n");
  fprintf(fp1, "        }\n");
  fprintf(fp1, "        geometry IndexedFaceSet {\n");
  fprintf(fp1, "                creaseAngle 1.0\n");
  fprintf(fp1, "                coord Coordinate {\n");
  fprintf(fp1, "                        point [\n");
  for (num = 0; num < v_num; num++) 
    fprintf(fp1, "%f %f %f\n",vertex[num].x*span[0]+orig[0],
	    vertex[num].y*span[1]+orig[1],vertex[num].z*span[2]+orig[2]);
  fprintf(fp1, "			]\n");
  fprintf(fp1, "		}\n");
  fprintf(fp1, "		coordIndex [\n");
  for (num = 0; num < t_num; num++) 
    fprintf(fp1, "%d %d %d -1\n",triangle[num].x,triangle[num].y,triangle[num].z);
  fprintf(fp1, "		]\n");
  fprintf(fp1, "	}\n");
  fprintf(fp1, "}\n");
  fclose(fp1);
  

  helix_pnts1 = (VECTOR*)malloc(sizeof(VECTOR)*PolyNum);
  helix_pnts2 = (VECTOR*)malloc(sizeof(VECTOR)*PolyNum);
  v_num = 0;
  t_num = 0;
  for (num = 0; num < HSeedNum; num++) {
    if (AllHelix_list[num].points == NULL)
      continue;
    else {
      pnt_obj = AllHelix_list[num].points;
      min_mx = pnt_obj->x;
      min_my = pnt_obj->y;
      min_mz = pnt_obj->z;
      min_nx = (pnt_obj->next)->x;
      min_ny = (pnt_obj->next)->y;
      min_nz = (pnt_obj->next)->z;
      tempx = min_nx-min_mx;
      tempy = min_ny-min_my;
      tempz = min_nz-min_mz;
      theta = atan2(tempy,tempx);
      phi = atan2(tempz, sqrt(tempx*tempx+tempy*tempy));
      a[0][0] = cos(0.5*PIE-phi)*cos(theta);
      a[0][1] = cos(0.5*PIE-phi)*sin(theta);
      a[0][2] = -sin(0.5*PIE-phi);
      a[1][0] = -sin(theta);
      a[1][1] = cos(theta);
      a[1][2] = 0;
      a[2][0] = sin(0.5*PIE-phi)*cos(theta);
      a[2][1] = sin(0.5*PIE-phi)*sin(theta);
      a[2][2] = cos(0.5*PIE-phi);
      b[0][0] = cos(0.5*PIE-phi)*cos(theta);
      b[0][1] = -sin(theta); 
      b[0][2] = sin(0.5*PIE-phi)*cos(theta); 
      b[1][0] = cos(0.5*PIE-phi)*sin(theta);
      b[1][1] = cos(theta);
      b[1][2] = sin(0.5*PIE-phi)*sin(theta);
      b[2][0] = -sin(0.5*PIE-phi);
      b[2][1] = 0;
      b[2][2] = cos(0.5*PIE-phi);
      xx = sqrt(tempx*tempx+tempy*tempy);
      if (xx == 0) {
	x = helix_draw+min_mx;
	y = min_my;
	z = min_mz;
      }
      else {
	x = -helix_draw*tempy/xx+min_mx;
	y = helix_draw*tempx/xx+min_my;
	z = min_mz;
      }
      helix_pnts1[0].x = x;
      helix_pnts1[0].y = y;
      helix_pnts1[0].z = z;
      x = x-min_mx;
      y = y-min_my;
      z = z-min_mz;
      xx = a[0][0]*x+a[0][1]*y+a[0][2]*z;
      yy = a[1][0]*x+a[1][1]*y+a[1][2]*z;
      zz = a[2][0]*x+a[2][1]*y+a[2][2]*z;
      for (m = 1; m < PolyNum; m++) {
	x = cos(2*PIE*(float)(m)/(float)(PolyNum))*xx - 
	  sin(2*PIE*(float)(m)/(float)(PolyNum))*yy;
	y = sin(2*PIE*(float)(m)/(float)(PolyNum))*xx + 
	  cos(2*PIE*(float)(m)/(float)(PolyNum))*yy;
	z = zz;
        helix_pnts1[m].x = b[0][0]*x+b[0][1]*y+b[0][2]*z+min_mx;
	helix_pnts1[m].y = b[1][0]*x+b[1][1]*y+b[1][2]*z+min_my;
	helix_pnts1[m].z = b[2][0]*x+b[2][1]*y+b[2][2]*z+min_mz;
      }

      start_x = min_mx;
      start_y = min_my;
      start_z = min_mz;
      template = helixwd;
      pnt_obj = pnt_obj->next;
      end_x = pnt_obj->x;
      end_y = pnt_obj->y;
      end_z = pnt_obj->z;
      while (1) {
	distance = sqrt((start_x-end_x)*(start_x-end_x)+(start_y-end_y)*(start_y-end_y)+
			(start_z-end_z)*(start_z-end_z));
	if (template <= distance) {
	  start_x = (end_x-start_x)*template/distance+start_x;
	  start_y = (end_y-start_y)*template/distance+start_y;
	  start_z = (end_z-start_z)*template/distance+start_z;
	  template = helixwd;
	  
	  AllEigenVect = GetEigenVector(start_x, start_y, start_z);
	  tempx = AllEigenVect.x3;
	  tempy = AllEigenVect.y3;
	  tempz = AllEigenVect.z3;
	  tmpx = start_x-min_mx;
	  tmpy = start_y-min_my;
	  tmpz = start_z-min_mz;
	  dist = sqrt(tmpx*tmpx+tmpy*tmpy+tmpz*tmpz);
	  tmpx /= dist;
	  tmpy /= dist;
	  tmpz /= dist;
	  angle = tmpx*tempx+tmpy*tempy+tmpz*tempz;
	  if (angle < 0)
	    angle = -angle;
	  for (m = 0; m < PolyNum; m++) {
	    dist = (helix_pnts1[m].x-start_x)*tempx+(helix_pnts1[m].y-start_y)*tempy+
	      (helix_pnts1[m].z-start_z)*tempz;
	    if (dist < 0)
	      dist = -dist;
	    dist /= angle;
	    helix_pnts2[m].x = dist*tmpx + helix_pnts1[m].x;
	    helix_pnts2[m].y = dist*tmpy + helix_pnts1[m].y;
	    helix_pnts2[m].z = dist*tmpz + helix_pnts1[m].z;
	  }
	  DrawLine2(min_mx,min_my,min_mz,start_x,start_y,start_z,helix_pnts1,helix_pnts2);
	  for (m = 0; m < PolyNum; m++) {
	    helix_pnts1[m].x = helix_pnts2[m].x;
	    helix_pnts1[m].y = helix_pnts2[m].y;
	    helix_pnts1[m].z = helix_pnts2[m].z;
	  }
	  min_mx = start_x;
	  min_my = start_y;
	  min_mz = start_z;
	}
	else {
	  template -= distance;
	  start_x = end_x;
	  start_y = end_y;
	  start_z = end_z;
	  pnt_obj = pnt_obj->next;
	  if (pnt_obj == NULL) 
	    break;
	  end_x = pnt_obj->x;
	  end_y = pnt_obj->y;
	  end_z = pnt_obj->z;
	}
      }

      tmpx = start_x-min_mx;
      tmpy = start_y-min_my;
      tmpz = start_z-min_mz;
      dist = sqrt(tmpx*tmpx+tmpy*tmpy+tmpz*tmpz);
      tmpx /= dist;
      tmpy /= dist;
      tmpz /= dist;
      distance = -9999.0;
      for (m = 0; m < PolyNum; m++) {
	dist = (helix_pnts1[m].x-min_mx)*tmpx+(helix_pnts1[m].y-min_my)*tmpy+
	  (helix_pnts1[m].z-min_mz)*tmpz;
	if (dist > distance) 
	  distance = dist;
      }
      dist = sqrt((start_x-min_mx)*(start_x-min_mx)+(start_y-min_my)*(start_y-min_my)+
		  (start_z-min_mz)*(start_z-min_mz));
      if (distance > dist) {
	start_x = (start_x-min_mx)*distance/dist+min_mx;
	start_y = (start_y-min_my)*distance/dist+min_my;
	start_z = (start_z-min_mz)*distance/dist+min_mz;
      }
      tmpx = start_x-min_mx;
      tmpy = start_y-min_my;
      tmpz = start_z-min_mz;
      dist = sqrt(tmpx*tmpx+tmpy*tmpy+tmpz*tmpz);
      tmpx /= dist;
      tmpy /= dist;
      tmpz /= dist;
      for (m = 0; m < PolyNum; m++) {
	dist = (helix_pnts1[m].x-start_x)*tmpx+(helix_pnts1[m].y-start_y)*tmpy+
	  (helix_pnts1[m].z-start_z)*tmpz;
	if (dist < 0)
	  dist = - dist;
	helix_pnts2[m].x = dist*tmpx + helix_pnts1[m].x;
	helix_pnts2[m].y = dist*tmpy + helix_pnts1[m].y;
	helix_pnts2[m].z = dist*tmpz + helix_pnts1[m].z;
      }
      DrawLine2(min_mx,min_my,min_mz,start_x,start_y,start_z,helix_pnts1,helix_pnts2);
      
    }
  }


  fprintf(fp3, "#VRML V2.0 utf8\n");
  fprintf(fp3, "Shape {\n");
  fprintf(fp3, "        appearance Appearance {\n");
  fprintf(fp3, "                material Material {\n");
  fprintf(fp3, "                        ambientIntensity 0.8\n");
  fprintf(fp3, "                        diffuseColor 0.0 0.7 0.99\n");
  fprintf(fp3, "                        specularColor	0.8 0.8 0.01\n");
  fprintf(fp3, "                        emissiveColor	0.0 0.01 0.01\n");
  fprintf(fp3, "                        shininess	0.5\n");
  fprintf(fp3, "                        transparency	0\n");
  fprintf(fp3, "                }\n");
  fprintf(fp3, "        }\n");
  fprintf(fp3, "        geometry IndexedFaceSet {\n");
  fprintf(fp3, "                creaseAngle 1.0\n");
  fprintf(fp3, "                coord Coordinate {\n");
  fprintf(fp3, "                        point [\n");
  for (num = 0; num < v_num; num++) 
    fprintf(fp3, "%f %f %f\n",vertex[num].x*span[0]+orig[0],
	    vertex[num].y*span[1]+orig[1],vertex[num].z*span[2]+orig[2]);
  fprintf(fp3, "			]\n");
  fprintf(fp3, "		}\n");
  fprintf(fp3, "		coordIndex [\n");
  for (num = 0; num < t_num; num++) 
    fprintf(fp3, "%d %d %d -1\n",triangle[num].x,triangle[num].y,triangle[num].z);
  fprintf(fp3, "		]\n");
  fprintf(fp3, "	}\n");
  fprintf(fp3, "}\n");
  fclose(fp3);
  
  
  free(vertex);
  free(triangle);
  free(AllSheet);
  free(AllHelix_list);
  free(SheetDistMatrix);
  
}


void TraceSheet(int sheet_index)
{
  int tempt_x, tempt_y, tempt_z;
  int nx,ny,nz;
  float ax,ay,az;
  float dx,dy,dz;
  float new_dx,new_dy,new_dz;
  float start_x,start_y,start_z;
  VECTOR EigenVector;
  PNTS *pnt_obj;
  float dist1,dist2,ratio;
  int ii;
  unsigned short parent;
  int cellVerts[12]; 
  unsigned char cellIndex;      
  int begin_pnt,end_pnt;
  float total;
  float thick1,thick2,thick3;
  
  
  pnt_obj = AllSheet[sheet_index].points;
  start_x = pnt_obj->x;
  start_y = pnt_obj->y;
  start_z = pnt_obj->z;
  stack[0].x = (unsigned short)(pnt_obj->x);
  stack[0].y = (unsigned short)(pnt_obj->y);
  stack[0].z = (unsigned short)(pnt_obj->z);
  stack[0].parent = 0;
  stack[0].dx = AllSheet[sheet_index].dx;
  stack[0].dy = AllSheet[sheet_index].dy;
  stack[0].dz = AllSheet[sheet_index].dz;
  begin_pnt = 0;
  end_pnt = 1;
  
  
  while (begin_pnt < end_pnt) {
    tempt_x = stack[begin_pnt].x;
    tempt_y = stack[begin_pnt].y;
    tempt_z = stack[begin_pnt].z;
    parent = stack[begin_pnt].parent;
    dx = stack[parent].dx;
    dy = stack[parent].dy;
    dz = stack[parent].dz;
    
    EigenVector = GetCenter(tempt_x,tempt_y,tempt_z);
    ax = EigenVector.x;
    ay = EigenVector.y;
    az = EigenVector.z;
    if (ax == 0 && ay == 0 && az == 0) {
      ax = start_x;
      ay = start_y;
      az = start_z;
    }
    
    EigenVector = GetMaxAxisAndThickness(ax,ay,az,&thick1,&thick2,&thick3);
    new_dx = EigenVector.x;
    new_dy = EigenVector.y;
    new_dz = EigenVector.z;
    if (new_dx*dx+new_dy*dy+new_dz*dz < 0) {
      new_dx = -new_dx;
      new_dy = -new_dy;
      new_dz = -new_dz;
    }

    if (new_dx*dx+new_dy*dy+new_dz*dz > 0.865) {
      new_dx = 0.75*new_dx+0.25*dx;
      new_dy = 0.75*new_dy+0.25*dy;
      new_dz = 0.75*new_dz+0.25*dz;
      total = sqrt(new_dx*new_dx+new_dy*new_dy+new_dz*new_dz);
      new_dx /= total;
      new_dy /= total;
      new_dz /= total;
      stack[begin_pnt].dx = new_dx;
      stack[begin_pnt].dy = new_dy;
      stack[begin_pnt].dz = new_dz;

      pnt_obj = (PNTS*)malloc(sizeof(PNTS));
      pnt_obj->x = ax;
      pnt_obj->y = ay;
      pnt_obj->z = az;
      pnt_obj->next = AllSheet[sheet_index].points;
      AllSheet[sheet_index].points = pnt_obj;
      AllSheet[sheet_index].thickness = (AllSheet[sheet_index].thickness*
					 AllSheet[sheet_index].size+thick1)/
	(float)(AllSheet[sheet_index].size+1);
      AllSheet[sheet_index].size += 1;
      AllSheet[sheet_index].dx += new_dx;
      AllSheet[sheet_index].dy += new_dy;
      AllSheet[sheet_index].dz += new_dz;
      
      
      for (nz=tempt_z; nz<=min2(ZDIM-1,tempt_z+1); nz++)
	for (ny=tempt_y; ny<=min2(YDIM-1,tempt_y+1); ny++) 
	  for (nx=tempt_x; nx<=min2(XDIM-1,tempt_x+1); nx++) {
	    if (mc_sign[IndexVect(nx,ny,nz)] == 0) {
	      total = (nx-ax)*new_dx+(ny-ay)*new_dy+(nz-az)*new_dz;
	      if (total >= 0)
		mc_sign[IndexVect(nx,ny,nz)] = 255;
	      else
		mc_sign[IndexVect(nx,ny,nz)] = 1;
	    }
	  }
      
      for (ii = 0; ii < 12; ii++)
	cellVerts[ii] = -1;

      cellIndex = 0;
      if (mc_sign[IndexVect(tempt_x,tempt_y,tempt_z)] == 255) cellIndex |= 1;
      if (mc_sign[IndexVect(tempt_x,tempt_y+1,tempt_z)] == 255) cellIndex |= 2;
      if (mc_sign[IndexVect(tempt_x+1,tempt_y+1,tempt_z)] == 255) cellIndex |= 4;
      if (mc_sign[IndexVect(tempt_x+1,tempt_y,tempt_z)] == 255) cellIndex |= 8;
      if (mc_sign[IndexVect(tempt_x,tempt_y,tempt_z+1)] == 255) cellIndex |= 16;
      if (mc_sign[IndexVect(tempt_x,tempt_y+1,tempt_z+1)] == 255) cellIndex |= 32;
      if (mc_sign[IndexVect(tempt_x+1,tempt_y+1,tempt_z+1)] == 255) cellIndex |= 64;
      if (mc_sign[IndexVect(tempt_x+1,tempt_y,tempt_z+1)] == 255) cellIndex |= 128;     
      
      if (edgeTable[cellIndex] & 1) {
	if (mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].y == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x;
	  vertex[v_num].y = (float)tempt_y+ratio;
	  vertex[v_num].z = (float)tempt_z;
	  cellVerts[0] = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].y = v_num;
	  v_num++;
	}
	else 
	  cellVerts[0] = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].y;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = max2(tempt_x-1,0);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = max2(tempt_z-1,0);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 2) {
	if (mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].x == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+ratio;
	  vertex[v_num].y = (float)tempt_y+1;
	  vertex[v_num].z = (float)tempt_z;
	  cellVerts[1]  = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].x = v_num;
	  v_num++;
	}
	else 
	  cellVerts[1] = mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].x;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = min2(YDIM-1,tempt_y+1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = max2(tempt_z-1,0);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 4) {
	if (mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].y == -1) {  
	  dist1 = (tempt_x+1-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+1;
	  vertex[v_num].y = (float)tempt_y+ratio;
	  vertex[v_num].z = (float)tempt_z;
	  cellVerts[2] = v_num;
	  mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].y = v_num;
	  v_num++;
	}
	else 
	  cellVerts[2] = mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].y;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = min2(XDIM-1,tempt_x+1);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = max2(tempt_z-1,0);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 8) {
	if (mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].x == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+ratio;
	  vertex[v_num].y = (float)tempt_y;
	  vertex[v_num].z = (float)tempt_z;
	  cellVerts[3]  = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].x = v_num;
	  v_num++;
	}
	else 
	  cellVerts[3] = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].x;

	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = max2(0,tempt_y-1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = max2(tempt_z-1,0);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,max2(tempt_z-1,0))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 16) {
	if (mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].y == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	      dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x;
	  vertex[v_num].y = (float)tempt_y+ratio;
	  vertex[v_num].z = (float)tempt_z+1;
	  cellVerts[4]  = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].y = v_num;
	  v_num++;
	}
	else 
	  cellVerts[4] = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].y;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = max2(tempt_x-1,0);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = min2(tempt_z+1,ZDIM-1);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 32) {
	if (mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z+1)].x == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+ratio;
	  vertex[v_num].y = (float)tempt_y+1;
	  vertex[v_num].z = (float)tempt_z+1;
	  cellVerts[5]  = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z+1)].x = v_num;
	  v_num++;
	}
	else 
	  cellVerts[5] = mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z+1)].x;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = min2(YDIM-1,tempt_y+1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = min2(tempt_z+1,ZDIM-1);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 64) {
	if (mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z+1)].y == -1) {  
	  dist1 = (tempt_x+1-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+1;
	  vertex[v_num].y = (float)tempt_y+ratio;
	  vertex[v_num].z = (float)tempt_z+1;
	  cellVerts[6]  = v_num;
	  mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z+1)].y = v_num;
	  v_num++;
	}
	else 
	  cellVerts[6] = mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z+1)].y;

	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = min2(XDIM-1,tempt_x+1);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = min2(tempt_z+1,ZDIM-1);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 128) {
	if (mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].x == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+ratio;
	  vertex[v_num].y = (float)tempt_y;
	  vertex[v_num].z = (float)tempt_z+1;
	  cellVerts[7]  = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].x = v_num;
	  v_num++;
	}
	else 
	  cellVerts[7] = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].x;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = max2(0,tempt_y-1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = min2(tempt_z+1,ZDIM-1);
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,tempt_y,min2(tempt_z+1,ZDIM-1))] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 256) {
	if (mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].z == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x;
	  vertex[v_num].y = (float)tempt_y;
	  vertex[v_num].z = (float)tempt_z+ratio;
	  cellVerts[8]  = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].z = v_num;
	  v_num++;
	}
	else 
	  cellVerts[8] = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].z;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = max2(0,tempt_y-1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = max2(tempt_x-1,0);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 512) {
	if (mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].z == -1) {  
	  dist1 = (tempt_x-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x;
	  vertex[v_num].y = (float)tempt_y+1;
	  vertex[v_num].z = (float)tempt_z+ratio;
	  cellVerts[9]  = v_num;
	  mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].z = v_num;
	  v_num++;
	}
	else 
	  cellVerts[9] = mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].z;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = min2(YDIM-1,tempt_y+1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = max2(tempt_x-1,0);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(max2(tempt_x-1,0),tempt_y,tempt_z)] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 1024) {
	if (mc_edge[IndexVect(tempt_x+1,tempt_y+1,tempt_z)].z == -1) {  
	  dist1 = (tempt_x+1-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y+1-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+1;
	  vertex[v_num].y = (float)tempt_y+1;
	  vertex[v_num].z = (float)tempt_z+ratio;
	  cellVerts[10]  = v_num;
	  mc_edge[IndexVect(tempt_x+1,tempt_y+1,tempt_z)].z = v_num;
	  v_num++;
	}
	else 
	  cellVerts[10] = mc_edge[IndexVect(tempt_x+1,tempt_y+1,tempt_z)].z;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = min2(YDIM-1,tempt_y+1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,min2(YDIM-1,tempt_y+1),tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = min2(XDIM-1,tempt_x+1);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] = 1;
	  }
	}
      }
      if (edgeTable[cellIndex] & 2048) {
	if (mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].z == -1) {  
	  dist1 = (tempt_x+1-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z-az)*new_dz;
	  if (dist1 < 0)
	    dist1 = -dist1;
	  dist2 = (tempt_x+1-ax)*new_dx+(tempt_y-ay)*new_dy+(tempt_z+1-az)*new_dz;
	  if (dist2 < 0)
	    dist2 = -dist2;
	  ratio = dist1/(dist1+dist2);
	  vertex[v_num].x = (float)tempt_x+1;
	  vertex[v_num].y = (float)tempt_y;
	  vertex[v_num].z = (float)tempt_z+ratio;
	  cellVerts[11]  = v_num;
	  mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].z = v_num;
	  v_num++;
	}
	else 
	  cellVerts[11] = mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].z;
	
	if (thick1 > min_sheet_wd && thick1 < max_sheet_wd &&
	    max2(thick1/thick2,thick1/thick3) < 
	    min2(thick3/thick2,thick2/thick3)) {
	  if (mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] == 0) {
	    stack[end_pnt].x = min2(XDIM-1,tempt_x+1);
	    stack[end_pnt].y = tempt_y;
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(min2(XDIM-1,tempt_x+1),tempt_y,tempt_z)] = 1;
	  }
	  if (mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] == 0) {
	    stack[end_pnt].x = tempt_x;
	    stack[end_pnt].y = max2(0,tempt_y-1);
	    stack[end_pnt].z = tempt_z;
	    stack[end_pnt].parent = begin_pnt;
	    end_pnt++;
	    mc_visited[IndexVect(tempt_x,max2(0,tempt_y-1),tempt_z)] = 1;
	  }
	}
      }
      
      ii = 0;
      while (triTable[cellIndex][ii] != -1) {
	triangle[t_num].x = cellVerts[triTable[cellIndex][ii++]];
	triangle[t_num].y = cellVerts[triTable[cellIndex][ii++]];
	triangle[t_num].z = cellVerts[triTable[cellIndex][ii++]];
	tri_index[t_num] = sheet_index;
	t_num++;
      }
    }
    
    begin_pnt++;
    
  }
}



VECTOR GetCenter(int tempt_x, int tempt_y, int tempt_z) 
{
  float ax,ay,az;
  VECTOR tmp;
  int number,edge_num;

  number = 0;
  ax = 0;
  ay = 0;
  az = 0;
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].x;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].y;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z)].z;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].y;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z)].z;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].x;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z)].z;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x+1,tempt_y+1,tempt_z)].z;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].x;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y,tempt_z+1)].y;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x+1,tempt_y,tempt_z+1)].y;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  edge_num = mc_edge[IndexVect(tempt_x,tempt_y+1,tempt_z+1)].x;
  if (edge_num != -1) {
    number++;
    ax += vertex[edge_num].x;
    ay += vertex[edge_num].y;
    az += vertex[edge_num].z;
  }
  
  if (number > 0) {
    tmp.x = ax/(float)number;
    tmp.y = ay/(float)number;
    tmp.z = az/(float)number;
  }
  else {
    tmp.x = 0;
    tmp.y = 0;
    tmp.z = 0;
  }

  
  return(tmp);
}




void DrawLine2(float sx,float sy,float sz,float ex,float ey,float ez,VECTOR *helix_pnts1,VECTOR *helix_pnts2)
{
  int m;
  

  for (m = 0; m < PolyNum; m++) {
    vertex[v_num+2*m].x = helix_pnts1[m].x;
    vertex[v_num+2*m].y = helix_pnts1[m].y;
    vertex[v_num+2*m].z = helix_pnts1[m].z;
    vertex[v_num+2*m+1].x = helix_pnts2[m].x;
    vertex[v_num+2*m+1].y = helix_pnts2[m].y;
    vertex[v_num+2*m+1].z = helix_pnts2[m].z;
  }
  
  for (m = 0; m < PolyNum-1; m++) {
    triangle[t_num+2*m].x = v_num+2*m;
    triangle[t_num+2*m].y = v_num+2*m+2;
    triangle[t_num+2*m].z = v_num+2*m+1;
    triangle[t_num+2*m+1].x = v_num+2*m+1;
    triangle[t_num+2*m+1].y = v_num+2*m+2;
    triangle[t_num+2*m+1].z = v_num+2*m+3;
  }

  triangle[t_num+2*PolyNum-2].x = v_num+2*PolyNum-2;
  triangle[t_num+2*PolyNum-2].y = v_num;
  triangle[t_num+2*PolyNum-2].z = v_num+2*PolyNum-1;
  triangle[t_num+2*PolyNum-1].x = v_num+2*PolyNum-1;
  triangle[t_num+2*PolyNum-1].y = v_num;
  triangle[t_num+2*PolyNum-1].z = v_num+1;
  
  vertex[v_num+2*PolyNum].x = sx;
  vertex[v_num+2*PolyNum].y = sy;
  vertex[v_num+2*PolyNum].z = sz;
  vertex[v_num+2*PolyNum+1].x = ex;
  vertex[v_num+2*PolyNum+1].y = ey;
  vertex[v_num+2*PolyNum+1].z = ez;

  for (m = 0; m < PolyNum-1; m++) {
    triangle[t_num+2*PolyNum+2*m].x = v_num+2*m;
    triangle[t_num+2*PolyNum+2*m].y = v_num+2*PolyNum;
    triangle[t_num+2*PolyNum+2*m].z = v_num+2*m+2;
    triangle[t_num+2*PolyNum+2*m+1].x = v_num+2*PolyNum+1;
    triangle[t_num+2*PolyNum+2*m+1].y = v_num+2*m+1;
    triangle[t_num+2*PolyNum+2*m+1].z = v_num+2*m+3;
  }

  triangle[t_num+4*PolyNum-2].x = v_num+2*PolyNum-2;
  triangle[t_num+4*PolyNum-2].y = v_num+2*PolyNum;
  triangle[t_num+4*PolyNum-2].z = v_num;
  triangle[t_num+4*PolyNum-1].x = v_num+2*PolyNum+1;
  triangle[t_num+4*PolyNum-1].y = v_num+2*PolyNum-1;
  triangle[t_num+4*PolyNum-1].z = v_num+1;
  
  v_num += 2*PolyNum+2;
  t_num += 4*PolyNum;

}
