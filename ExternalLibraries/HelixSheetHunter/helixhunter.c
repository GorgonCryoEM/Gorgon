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
#define PIE           3.1415927
#define WINDOW 3
#define IndexVect(i,j,k) ((k)*XDIM*YDIM + (j)*XDIM + (i))


SEEDS *AllHelix;
int HelixSeedNum;

int v_num, t_num;
VECTOR *vertex;
INTVECT *triangle;

float t_low;
int XDIM, YDIM, ZDIM;
float *dataset;
VECTOR* grad_vec;
float helix_wd;
float *HelixDistMatrix;


VECTOR GetMinAxisAndThickness(float,float,float,float*,float*,float*);
void DrawLine(float, float, float, float, float, float, float);
void LocateMaximum(float*, float*, float*);
void TraceHelix(int);
void RefineHelix(int);
void MergeHelix(int, int);
float GetDistance(float *, float *, float *, int);
float GetThickness(float, float, float,float, float, float);
EIGENVECT GetEigenVector(float, float, float);


void HelixHunter(int xd,int yd,int zd,float *span,float *orig,float *data, 
		 unsigned char *seed,VECTOR* gradvec,float tlow,float helixwd,
		 float helixlength, FILE *fp1, SEEDS *AllHelix_list, int HSeedNum)
{
  int i,j,k;
  int num,m,n;
  int tm,tn;
  VECTOR EigenVector;
  PNTS *pnt_obj,*pnt_tmp;
  float tmpx,tmpy,tmpz;
  float thick1,thick2,thick3;
  float tempx,tempy,tempz;
  float distance,min_dist;
  float dist1,dist2,angle;
  int alpha_num;
  float min_mx,min_my,min_mz;
  float min_nx,min_ny,min_nz;
  

  XDIM = xd;
  YDIM = yd;
  ZDIM = zd;
  dataset = data;
  grad_vec = gradvec;
  t_low = tlow;
  helix_wd = helixwd;
  HelixSeedNum = HSeedNum;
  AllHelix = AllHelix_list;
  
 
  vertex = (VECTOR*)malloc(sizeof(VECTOR)*50000*PolyNum);
  triangle = (INTVECT*)malloc(sizeof(INTVECT)*50000*PolyNum);
  v_num = 0;
  t_num = 0;

  
  HelixDistMatrix = (float*)malloc(sizeof(float)*HelixSeedNum*HelixSeedNum);
  for (j = 0; j < HelixSeedNum; j++) 
    for (i = 0; i < HelixSeedNum; i++)
      HelixDistMatrix[j*HelixSeedNum+i] = 9999999.0f;
  

  alpha_num = 0;
  for (k=0; k<ZDIM; k++)
    for (j=0; j<YDIM; j++) 
      for (i=0; i<XDIM; i++) {
	if (seed[IndexVect(i,j,k)] == 111) {
	  seed[IndexVect(i,j,k)] = 0;
	  tmpx = (float)i;
	  tmpy = (float)j;
	  tmpz = (float)k;
	  LocateMaximum(&tmpx,&tmpy,&tmpz);
	  EigenVector = GetMinAxisAndThickness(tmpx,tmpy,tmpz,&thick1,&thick2,&thick3);
	  /*
	  DrawLine(tmpx-0.5*EigenVector.x,tmpy-0.5*EigenVector.y,tmpz-0.5*EigenVector.z,
	           tmpx+0.5*EigenVector.x,tmpy+0.5*EigenVector.y,tmpz+0.5*EigenVector.z,0.5);
	  */
	  AllHelix[alpha_num].dx = EigenVector.x;
	  AllHelix[alpha_num].dy = EigenVector.y;
	  AllHelix[alpha_num].dz = EigenVector.z;
	  AllHelix[alpha_num].thickness = (thick1+thick2)*0.5;
	  AllHelix[alpha_num].size = 1;
	  pnt_obj = (PNTS*)malloc(sizeof(PNTS));
	  pnt_obj->x = tmpx;
	  pnt_obj->y = tmpy;
	  pnt_obj->z = tmpz;
	  pnt_obj->next = NULL;
	  AllHelix[alpha_num].points = pnt_obj;
	  TraceHelix(alpha_num);
	  alpha_num++;
	}
      }
  printf("alpha_num = %d;\n",alpha_num);
 
  
  for (num = 0; num < HelixSeedNum; num++) 
    RefineHelix(num);
  
  for (n = 0; n < HelixSeedNum; n++) {
    pnt_obj = AllHelix[n].points;
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
    	  
    if (distance < helixlength) {
      AllHelix[n].points = NULL;
      AllHelix[n].size = 0;
    }
  }
  
  
  for (n = 0; n < HelixSeedNum; n++) {
    HelixDistMatrix[n*HelixSeedNum+n] = 99999.0;
    for (m = n+1; m < HelixSeedNum; m++) {
      min_dist = 99999.0;
      pnt_obj = AllHelix[n].points;
      while (pnt_obj != NULL) {
	tmpx = pnt_obj->x;
	tmpy = pnt_obj->y;
	tmpz = pnt_obj->z;
	pnt_tmp = AllHelix[m].points;
	while (pnt_tmp != NULL) {
	  tempx = pnt_tmp->x;
	  tempy = pnt_tmp->y;
	  tempz = pnt_tmp->z;
	  distance = sqrt((tmpx-tempx)*(tmpx-tempx)+(tmpy-tempy)*(tmpy-tempy)+
			  (tmpz-tempz)*(tmpz-tempz));
	  if (distance < min_dist) 
	    min_dist = distance;
	  pnt_tmp = pnt_tmp->next;
	}
	pnt_obj = pnt_obj->next;
      }

      HelixDistMatrix[n*HelixSeedNum+m] = min_dist;
      HelixDistMatrix[m*HelixSeedNum+n] = min_dist;
    }
  }

  while (1) {
    min_dist = 99999.0;
    for (tn = 0; tn < HelixSeedNum; tn++) 
      for (tm = tn+1; tm < HelixSeedNum; tm++) {
	if (HelixDistMatrix[tn*HelixSeedNum+tm] < min_dist) {
	  min_dist = HelixDistMatrix[tn*HelixSeedNum+tm];
	  m = tm;
	  n = tn;
	}
      }
     
    if (min_dist > helix_wd) 
      break;
    else {
      min_dist = 99999.0;
      pnt_obj = AllHelix[n].points;
      while (pnt_obj != NULL) {
	tmpx = pnt_obj->x;
	tmpy = pnt_obj->y;
	tmpz = pnt_obj->z;
	pnt_tmp = AllHelix[m].points;
	while (pnt_tmp != NULL) {
	  tempx = pnt_tmp->x;
	  tempy = pnt_tmp->y;
	  tempz = pnt_tmp->z;
	  distance = sqrt((tmpx-tempx)*(tmpx-tempx)+(tmpy-tempy)*(tmpy-tempy)+
			  (tmpz-tempz)*(tmpz-tempz));
	  if (distance < min_dist) {
	    min_dist = distance;
	    min_nx = tmpx;
	    min_ny = tmpy;
	    min_nz = tmpz;
	    min_mx = tempx;
	    min_my = tempy;
	    min_mz = tempz;
	  }
	  pnt_tmp = pnt_tmp->next;
	}
	pnt_obj = pnt_obj->next;
      }
      
      tmpx = AllHelix[n].dx;
      tmpy = AllHelix[n].dy;
      tmpz = AllHelix[n].dz;
      distance = sqrt(tmpx*tmpx+tmpy*tmpy+tmpz*tmpz);
      tmpx /= distance;
      tmpy /= distance;
      tmpz /= distance;
      tempx = AllHelix[m].dx;
      tempy = AllHelix[m].dy;
      tempz = AllHelix[m].dz;
      distance = sqrt(tempx*tempx+tempy*tempy+tempz*tempz);
      tempx /= distance;
      tempy /= distance;
      tempz /= distance;
      angle = tmpx*tempx+tmpy*tempy+tmpz*tempz;
      if (angle < 0)
	angle = -angle;
      
      if (angle > 0.75)
	MergeHelix(n,m);
      else {
	pnt_obj = AllHelix[n].points;
	dist1 = sqrt((min_nx-pnt_obj->x)*(min_nx-pnt_obj->x)+
		     (min_ny-pnt_obj->y)*(min_ny-pnt_obj->y)+
		     (min_nz-pnt_obj->z)*(min_nz-pnt_obj->z));
	while (pnt_obj->next != NULL) 
	  pnt_obj = pnt_obj->next;
	distance = sqrt((min_nx-pnt_obj->x)*(min_nx-pnt_obj->x)+
			(min_ny-pnt_obj->y)*(min_ny-pnt_obj->y)+
			(min_nz-pnt_obj->z)*(min_nz-pnt_obj->z));
	if (distance < dist1)
	  dist1 = distance;
	pnt_obj = AllHelix[m].points;
	dist2 = sqrt((min_mx-pnt_obj->x)*(min_mx-pnt_obj->x)+
		     (min_my-pnt_obj->y)*(min_my-pnt_obj->y)+
		     (min_mz-pnt_obj->z)*(min_mz-pnt_obj->z));
	while (pnt_obj->next != NULL) 
	  pnt_obj = pnt_obj->next;
	distance = sqrt((min_mx-pnt_obj->x)*(min_mx-pnt_obj->x)+
			(min_my-pnt_obj->y)*(min_my-pnt_obj->y)+
			(min_mz-pnt_obj->z)*(min_mz-pnt_obj->z));
	if (distance < dist2)
	  dist2 = distance;
	if (dist1 > dist2)
	  distance = dist1;
	else
	  distance = dist2;
	
	if (distance < helix_wd) {
	  HelixDistMatrix[m*HelixSeedNum+n] = 99999.0;
	  HelixDistMatrix[n*HelixSeedNum+m] = 99999.0;
	  continue;
	}
	else {
	  thick1 = AllHelix[n].thickness - helix_wd;
	  if (thick1 < 0)
	    thick1 = -thick1;
	  thick2 = AllHelix[m].thickness - helix_wd;
	  if (thick2 < 0)
	    thick2 = -thick2;
	  if (thick1 > thick2) {
	    AllHelix[n].points = NULL;
	    for (tm = 0; tm < HelixSeedNum; tm++) {
	      HelixDistMatrix[n*HelixSeedNum+tm] = 99999.0;
	      HelixDistMatrix[tm*HelixSeedNum+n] = 99999.0;
	    }
	  }
	  else {
	    AllHelix[m].points = NULL;
	    for (tm = 0; tm < HelixSeedNum; tm++) {
	      HelixDistMatrix[m*HelixSeedNum+tm] = 99999.0;
	      HelixDistMatrix[tm*HelixSeedNum+m] = 99999.0;
	    }
	  }
	}
      }
    }
  }
  
  
  for (num = 0; num < HelixSeedNum; num++) {
    if (AllHelix[num].points == NULL)
      continue;
    else {
      pnt_obj = AllHelix[num].points;
      tmpx = pnt_obj->x;
      tmpy = pnt_obj->y;
      tmpz = pnt_obj->z;
      while (pnt_obj->next != NULL) {
	pnt_obj = pnt_obj->next;
	tempx = pnt_obj->x;
	tempy = pnt_obj->y;
	tempz = pnt_obj->z;
	DrawLine(tmpx,tmpy,tmpz,tempx,tempy,tempz,0.1);
	tmpx = tempx;
	tmpy = tempy;
	tmpz = tempz;
      }
    }
  }


  fprintf(fp1, "#VRML V2.0 utf8\n");
  fprintf(fp1, "Shape {\n");
  fprintf(fp1, "        appearance Appearance {\n");
  fprintf(fp1, "                material Material {\n");
  fprintf(fp1, "                        ambientIntensity 0.8\n");
  fprintf(fp1, "                        diffuseColor 0.0 0.7 0.99\n");
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
  
  free(vertex);
  free(triangle);
  free(HelixDistMatrix);

}



void MergeHelix(int n, int m)
{
  PNTS *pnt_obj,*pnt_nxt,*pnt_prv;
  float angle;
  float tmpx,tmpy,tmpz;
  float tempx,tempy,tempz;
  float distance,weight,min_dist;
  int num;


  angle = AllHelix[n].dx*AllHelix[m].dx+
    AllHelix[n].dy*AllHelix[m].dy+
    AllHelix[n].dz*AllHelix[m].dz;
  if (angle < 0) {
    if (AllHelix[n].size <= AllHelix[m].size) {
      pnt_prv = AllHelix[n].points;
      pnt_obj = pnt_prv->next;
      pnt_prv->next = NULL;
      while (pnt_obj != NULL) {
	pnt_nxt = pnt_obj->next;
	pnt_obj->next = pnt_prv;
	pnt_prv = pnt_obj;
	pnt_obj = pnt_nxt;
      }
      AllHelix[n].points = pnt_prv;
      AllHelix[n].dx = -AllHelix[n].dx;
      AllHelix[n].dy = -AllHelix[n].dy;
      AllHelix[n].dz = -AllHelix[n].dz;
    }
    else {
      pnt_prv = AllHelix[m].points;
      pnt_obj = pnt_prv->next;
      pnt_prv->next = NULL;
      while (pnt_obj != NULL) {
	pnt_nxt = pnt_obj->next;
	pnt_obj->next = pnt_prv;
	pnt_prv = pnt_obj;
	pnt_obj = pnt_nxt;
      }
      AllHelix[m].points = pnt_prv;
      AllHelix[m].dx = -AllHelix[m].dx;
      AllHelix[m].dy = -AllHelix[m].dy;
      AllHelix[m].dz = -AllHelix[m].dz;
    }
  }

  pnt_obj = AllHelix[n].points;
  tmpx = pnt_obj->x;
  tmpy = pnt_obj->y;
  tmpz = pnt_obj->z;
  pnt_obj = AllHelix[m].points;
  tempx = pnt_obj->x;
  tempy = pnt_obj->y;
  tempz = pnt_obj->z;
  angle = (tmpx-tempx)*AllHelix[m].dx +
    (tmpy-tempy)*AllHelix[m].dy +
    (tmpz-tempz)*AllHelix[m].dz;
  if (angle <= 0) {
    pnt_obj = AllHelix[n].points;
    while (pnt_obj != NULL) {
      tmpx = pnt_obj->x;
      tmpy = pnt_obj->y;
      tmpz = pnt_obj->z;
      distance = GetDistance(&tmpx,&tmpy,&tmpz, m);
      weight = min2(0.5,exp(-distance/helix_wd));
      pnt_obj->x = (1.0-weight)*pnt_obj->x+weight*tmpx;
      pnt_obj->y = (1.0-weight)*pnt_obj->y+weight*tmpy;
      pnt_obj->z = (1.0-weight)*pnt_obj->z+weight*tmpz;
      pnt_prv = pnt_obj;
      pnt_obj = pnt_obj->next;
    }
    pnt_nxt = AllHelix[m].points;
    pnt_nxt = pnt_nxt->next;
    pnt_prv->next = pnt_nxt;
    tmpx = pnt_prv->x;
    tmpy = pnt_prv->y;
    tmpz = pnt_prv->z;
    num = 0;
    while (pnt_nxt != NULL) {
      num++;
      tempx = pnt_nxt->x;
      tempy = pnt_nxt->y;
      tempz = pnt_nxt->z;
      distance = sqrt((tempx-tmpx)*(tempx-tmpx)+
		      (tempy-tmpy)*(tempy-tmpy)+(tempz-tmpz)*(tempz-tmpz));
      weight = min2(0.5,exp(-distance/helix_wd));
      pnt_nxt->x = (1.0-weight)*pnt_nxt->x+weight*tmpx;
      pnt_nxt->y = (1.0-weight)*pnt_nxt->y+weight*tmpy;
      pnt_nxt->z = (1.0-weight)*pnt_nxt->z+weight*tmpz;
      pnt_nxt = pnt_nxt->next;
    }
    AllHelix[n].thickness = (AllHelix[n].thickness*AllHelix[n].size+
			     AllHelix[m].thickness*AllHelix[m].size)/
                            (float)(AllHelix[n].size+AllHelix[m].size);
    AllHelix[n].dx += AllHelix[m].dx;
    AllHelix[n].dy += AllHelix[m].dy;
    AllHelix[n].dz += AllHelix[m].dz;
    AllHelix[n].size += num;
    AllHelix[m].points = NULL;
    for (num = 0; num < HelixSeedNum; num++) {
      if (num!=n && num!=m) {
	min_dist = 99999.0;
	pnt_obj = AllHelix[n].points;
	while (pnt_obj != NULL) {
	  tmpx = pnt_obj->x;
	  tmpy = pnt_obj->y;
	  tmpz = pnt_obj->z;
	  pnt_prv = AllHelix[num].points;
	  while (pnt_prv != NULL) {
	    tempx = pnt_prv->x;
	    tempy = pnt_prv->y;
	    tempz = pnt_prv->z;
	    distance = sqrt((tmpx-tempx)*(tmpx-tempx)+(tmpy-tempy)*(tmpy-tempy)+
			    (tmpz-tempz)*(tmpz-tempz));
	    if (distance < min_dist) 
	      min_dist = distance;
	    pnt_prv = pnt_prv->next;
	  }
	  pnt_obj = pnt_obj->next;
	}
	HelixDistMatrix[n*HelixSeedNum+num] = min_dist;
	HelixDistMatrix[num*HelixSeedNum+n] = min_dist;
      }
      HelixDistMatrix[m*HelixSeedNum+num] = 99999.0;
      HelixDistMatrix[num*HelixSeedNum+m] = 99999.0;	
    }
  }
  else {
    pnt_obj = AllHelix[m].points;
    while (pnt_obj != NULL) {
      tmpx = pnt_obj->x;
      tmpy = pnt_obj->y;
      tmpz = pnt_obj->z;
      distance = GetDistance(&tmpx,&tmpy,&tmpz, n);
      weight = min2(0.5,exp(-distance/helix_wd));
      pnt_obj->x = (1.0-weight)*pnt_obj->x+weight*tmpx;
      pnt_obj->y = (1.0-weight)*pnt_obj->y+weight*tmpy;
      pnt_obj->z = (1.0-weight)*pnt_obj->z+weight*tmpz;
      pnt_prv = pnt_obj;
      pnt_obj = pnt_obj->next;
    }
    pnt_nxt = AllHelix[n].points;
    pnt_nxt = pnt_nxt->next;
    pnt_prv->next = pnt_nxt;
    tmpx = pnt_prv->x;
    tmpy = pnt_prv->y;
    tmpz = pnt_prv->z;
    num = 0;
    while (pnt_nxt != NULL) {
      num++;
      tempx = pnt_nxt->x;
      tempy = pnt_nxt->y;
      tempz = pnt_nxt->z;
      distance = sqrt((tempx-tmpx)*(tempx-tmpx)+
		      (tempy-tmpy)*(tempy-tmpy)+(tempz-tmpz)*(tempz-tmpz));
      weight = min2(0.5,exp(-distance/helix_wd));
      pnt_nxt->x = (1.0-weight)*pnt_nxt->x+weight*tmpx;
      pnt_nxt->y = (1.0-weight)*pnt_nxt->y+weight*tmpy;
      pnt_nxt->z = (1.0-weight)*pnt_nxt->z+weight*tmpz;
      pnt_nxt = pnt_nxt->next;
    }
    AllHelix[m].thickness = (AllHelix[n].thickness*AllHelix[n].size+
			     AllHelix[m].thickness*AllHelix[m].size)/
                            (float)(AllHelix[n].size+AllHelix[m].size);
    AllHelix[m].dx += AllHelix[n].dx;
    AllHelix[m].dy += AllHelix[n].dy;
    AllHelix[m].dz += AllHelix[n].dz;
    AllHelix[m].size += num;
    AllHelix[n].points = NULL;
    for (num = 0; num < HelixSeedNum; num++) {
      if (num!=m && num!=n) {
	min_dist = 99999.0;
	pnt_obj = AllHelix[m].points;
	while (pnt_obj != NULL) {
	  tmpx = pnt_obj->x;
	  tmpy = pnt_obj->y;
	  tmpz = pnt_obj->z;
	  pnt_prv = AllHelix[num].points;
	  while (pnt_prv != NULL) {
	    tempx = pnt_prv->x;
	    tempy = pnt_prv->y;
	    tempz = pnt_prv->z;
	    distance = sqrt((tmpx-tempx)*(tmpx-tempx)+(tmpy-tempy)*(tmpy-tempy)+
			    (tmpz-tempz)*(tmpz-tempz));
	    if (distance < min_dist) 
	      min_dist = distance;
	    pnt_prv = pnt_prv->next;
	  }
	  pnt_obj = pnt_obj->next;
	}
	HelixDistMatrix[m*HelixSeedNum+num] = min_dist;
	HelixDistMatrix[num*HelixSeedNum+m] = min_dist;
      }
      HelixDistMatrix[n*HelixSeedNum+num] = 99999.0;
      HelixDistMatrix[num*HelixSeedNum+n] = 99999.0;	
    }
  }
  
}



float GetDistance(float *ax, float *ay, float *az, int n)
{
  float tmpx,tmpy,tmpz;
  float tempx,tempy,tempz;
  float px,py,pz;
  float qx,qy,qz;
  float a,b,c,d;
  float min_dist;
  PNTS *pnt_obj,*pnt_tmp;


  tmpx = *ax;
  tmpy = *ay;
  tmpz = *az;
             
  pnt_obj = AllHelix[n].points;
  while (pnt_obj != NULL && pnt_obj->next != NULL) {
    px = pnt_obj->x;
    py = pnt_obj->y;
    pz = pnt_obj->z;
    pnt_tmp = pnt_obj->next;
    qx = pnt_tmp->x;
    qy = pnt_tmp->y;
    qz = pnt_tmp->z;
    a = (px-qx)*(px-qx)+(py-qy)*(py-qy)+(pz-qz)*(pz-qz);
    b = (px-tmpx)*(px-tmpx)+(py-tmpy)*(py-tmpy)+(pz-tmpz)*(pz-tmpz);
    c = (qx-tmpx)*(qx-tmpx)+(qy-tmpy)*(qy-tmpy)+(qz-tmpz)*(qz-tmpz);
    if (c >= a+b) {
      min_dist = sqrt((px-tmpx)*(px-tmpx)+
		      (py-tmpy)*(py-tmpy)+(pz-tmpz)*(pz-tmpz));
      *ax = px;
      *ay = py;
      *az = pz;
      break;
    }
    else if (b >= a+c) 
      pnt_obj = pnt_obj->next;
    else {
      d = 0.5*(a+b-c)/sqrt(a);
      a = sqrt(a);
      tempx = (qx-px)*d/a+px;
      tempy = (qy-py)*d/a+py;
      tempz = (qz-pz)*d/a+pz;
      *ax = tempx;
      *ay = tempy;
      *az = tempz;
      min_dist = sqrt((tempx-tmpx)*(tempx-tmpx)+
		      (tempy-tmpy)*(tempy-tmpy)+(tempz-tmpz)*(tempz-tmpz));
      break;
    }
  }
  if (pnt_obj->next == NULL) {
    min_dist = sqrt((qx-tmpx)*(qx-tmpx)+
		    (qy-tmpy)*(qy-tmpy)+(qz-tmpz)*(qz-tmpz));
    *ax = qx;
    *ay = qy;
    *az = qz;
  }

  AllHelix[n].points = pnt_obj;
  return(min_dist);
}


void TraceHelix(int helix_index)
{
  float ax,ay,az;
  float v1,v2,v3,density;
  float tx,ty,tz;
  float dx,dy,dz;
  float start_x,start_y,start_z;
  float start_dx,start_dy,start_dz;
  float new_dx,new_dy,new_dz;
  float x00,x01,x10,x11,y0,y1;
  float angle, length;
  VECTOR EigenVector;
  PNTS *pnt_obj,*pnt_tmp;
  

  start_dx = AllHelix[helix_index].dx;
  start_dy = AllHelix[helix_index].dy;
  start_dz = AllHelix[helix_index].dz;
  pnt_obj = AllHelix[helix_index].points;
  start_x = pnt_obj->x;
  start_y = pnt_obj->y;
  start_z = pnt_obj->z;
  
  dx = start_dx;
  dy = start_dy;
  dz = start_dz;
  ax = start_x;
  ay = start_y;
  az = start_z;
  while (1) {
    tx = ax + dx;
    ty = ay + dy;
    tz = az + dz;
    LocateMaximum(&tx,&ty,&tz);
    
    if (tx==ax && ty==ay && tz==az) {
      tx = ax + 0.5*dx;
      ty = ay + 0.5*dy;
      tz = az + 0.5*dz;
    }
    else {
      tx = 0.5*(ax + dx + tx);
      ty = 0.5*(ay + dy + ty);
      tz = 0.5*(az + dz + tz);
    }
    
    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
    y0  = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
    y1  = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
    density = y0*((int)tz+1-tz) + y1*(tz-(int)tz);
    if (density > t_low) {
      EigenVector = GetMinAxisAndThickness(tx,ty,tz,&v1,&v2,&v3);
      new_dx = EigenVector.x;
      new_dy = EigenVector.y;
      new_dz = EigenVector.z;
      dx = AllHelix[helix_index].dx;
      dy = AllHelix[helix_index].dy;
      dz = AllHelix[helix_index].dz;
      length = sqrt(dx*dx+dy*dy+dz*dz);
      dx /= length;
      dy /= length;
      dz /= length;
      angle = new_dx*dx+new_dy*dy+new_dz*dz;
      if (angle < 0) 
	angle = -angle;
      angle = 1.0-2.0*acos(angle)/PIE;
      angle = angle*angle*angle;
      
      if ((v1+v2)*0.5 > (1.0-0.333*angle)*AllHelix[helix_index].thickness && 
	  (v1+v2)*0.5 < (1.0+0.333*angle)*AllHelix[helix_index].thickness && 
	  min2(v1/v2,v2/v1) > max2(v1/v3,v2/v3)) {
	
	if (new_dx*dx+new_dy*dy+new_dz*dz < 0) {
	  new_dx = -new_dx;
	  new_dy = -new_dy;
	  new_dz = -new_dz;
	}
	AllHelix[helix_index].dx += new_dx;
	AllHelix[helix_index].dy += new_dy;
	AllHelix[helix_index].dz += new_dz;
	AllHelix[helix_index].thickness = (AllHelix[helix_index].thickness*
	  AllHelix[helix_index].size+(v1+v2)*0.5)/(float)(AllHelix[helix_index].size+1);
	AllHelix[helix_index].size += 1;
	pnt_obj = (PNTS*)malloc(sizeof(PNTS));
	pnt_obj->x = tx;
	pnt_obj->y = ty;
	pnt_obj->z = tz;
	pnt_obj->next = NULL;
	pnt_tmp = AllHelix[helix_index].points;
	while (pnt_tmp->next != NULL) 
	  pnt_tmp = pnt_tmp->next;
	pnt_tmp->next = pnt_obj;
	
	ax = tx;
	ay = ty;
	az = tz;
	dx = new_dx;
	dy = new_dy;
	dz = new_dz;
      }
      else 
	break;
    }
    else
      break;
  }


  dx = -start_dx;
  dy = -start_dy;
  dz = -start_dz;
  ax = start_x;
  ay = start_y;
  az = start_z;
  while (1) {
    tx = ax + dx;
    ty = ay + dy;
    tz = az + dz;
    LocateMaximum(&tx,&ty,&tz);
    
    if (tx==ax && ty==ay && tz==az) {
      tx = ax + 0.5*dx;
      ty = ay + 0.5*dy;
      tz = az + 0.5*dz;
    }
    else {
      tx = 0.5*(ax + dx + tx);
      ty = 0.5*(ay + dy + ty);
      tz = 0.5*(az + dz + tz);
    }
    
    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
    y0  = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
    y1  = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
    density = y0*((int)tz+1-tz) + y1*(tz-(int)tz);
    if (density > t_low) {
      EigenVector = GetMinAxisAndThickness(tx,ty,tz,&v1,&v2,&v3);
      new_dx = EigenVector.x;
      new_dy = EigenVector.y;
      new_dz = EigenVector.z;
      dx = AllHelix[helix_index].dx;
      dy = AllHelix[helix_index].dy;
      dz = AllHelix[helix_index].dz;
      length = sqrt(dx*dx+dy*dy+dz*dz);
      dx /= length;
      dy /= length;
      dz /= length;
      angle = new_dx*dx+new_dy*dy+new_dz*dz;
      if (angle < 0) 
	angle = -angle;
      angle = 1.0-2.0*acos(angle)/PIE;
      angle = angle*angle*angle;
      
      if ((v1+v2)*0.5 > (1.0-0.333*angle)*AllHelix[helix_index].thickness && 
	  (v1+v2)*0.5 < (1.0+0.333*angle)*AllHelix[helix_index].thickness && 
	  min2(v1/v2,v2/v1) > max2(v1/v3,v2/v3)) {
	
	if (new_dx*dx+new_dy*dy+new_dz*dz < 0) {
	  new_dx = -new_dx;
	  new_dy = -new_dy;
	  new_dz = -new_dz;
	}
	AllHelix[helix_index].dx += new_dx;
	AllHelix[helix_index].dy += new_dy;
	AllHelix[helix_index].dz += new_dz;
	AllHelix[helix_index].thickness = (AllHelix[helix_index].thickness*
	  AllHelix[helix_index].size+(v1+v2)*0.5)/(float)(AllHelix[helix_index].size+1);
	AllHelix[helix_index].size += 1;
	pnt_obj = (PNTS*)malloc(sizeof(PNTS));
	pnt_obj->x = tx;
	pnt_obj->y = ty;
	pnt_obj->z = tz;
	pnt_obj->next = AllHelix[helix_index].points;
	AllHelix[helix_index].points = pnt_obj;
	
	ax = tx;
	ay = ty;
	az = tz;
	dx = -new_dx;
	dy = -new_dy;
	dz = -new_dz;
      }
      else
	break;
    }
    else
      break;
  }
  
}




void RefineHelix(int helix_index)
{
  float ax,ay,az;
  float v1,v2,v3,density;
  float tx,ty,tz;
  float dx,dy,dz;
  float start_x,start_y,start_z;
  float start_dx,start_dy,start_dz;
  float new_dx,new_dy,new_dz;
  float x00,x01,x10,x11,y0,y1;
  float angle, length;
  EIGENVECT EigenVector;
  PNTS *pnt_obj,*pnt_tmp;
  

  start_dx = AllHelix[helix_index].dx;
  start_dy = AllHelix[helix_index].dy;
  start_dz = AllHelix[helix_index].dz;
  length = sqrt(start_dx*start_dx+start_dy*start_dy+start_dz*start_dz);
  start_dx /= length;
  start_dy /= length;
  start_dz /= length;
  pnt_obj = AllHelix[helix_index].points;
  while (pnt_obj->next != NULL) 
    pnt_obj = pnt_obj->next;
  start_x = pnt_obj->x;
  start_y = pnt_obj->y;
  start_z = pnt_obj->z;
  	
  dx = start_dx;
  dy = start_dy;
  dz = start_dz;
  ax = start_x;
  ay = start_y;
  az = start_z;
  while (1) {
    tx = ax + 0.5*dx;
    ty = ay + 0.5*dy;
    tz = az + 0.5*dz;
    
    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
    y0  = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
    y1  = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
    density = y0*((int)tz+1-tz) + y1*(tz-(int)tz);
    if (density > t_low) {
      EigenVector = GetEigenVector(tx, ty, tz);
      v1 = GetThickness(tx, ty, tz,EigenVector.x1,EigenVector.y1,EigenVector.z1);
      v1 += GetThickness(tx, ty, tz,-EigenVector.x1,-EigenVector.y1,-EigenVector.z1);
      v2 = GetThickness(tx, ty, tz,EigenVector.x2,EigenVector.y2,EigenVector.z2);
      v2 += GetThickness(tx, ty, tz,-EigenVector.x2,-EigenVector.y2,-EigenVector.z2);
      v3 = GetThickness(tx, ty, tz,EigenVector.x3,EigenVector.y3,EigenVector.z3);
      v3 += GetThickness(tx, ty, tz,-EigenVector.x3,-EigenVector.y3,-EigenVector.z3);
      v1 *= 0.5;
      v2 *= 0.5;
      v3 *= 0.5;
      
      new_dx = EigenVector.x3;
      new_dy = EigenVector.y3;
      new_dz = EigenVector.z3;
      dx = AllHelix[helix_index].dx;
      dy = AllHelix[helix_index].dy;
      dz = AllHelix[helix_index].dz;
      length = sqrt(dx*dx+dy*dy+dz*dz);
      dx /= length;
      dy /= length;
      dz /= length;
      angle = new_dx*dx+new_dy*dy+new_dz*dz;
      if (angle < 0) 
	angle = -angle;
      angle = 1.0-2.0*acos(angle)/PIE;
      angle = angle*angle*angle;
      if ((v1+v2)*0.5 > (1.0-0.9*angle)*AllHelix[helix_index].thickness && 
	  (v1+v2)*0.5 < (1.0+0.9*angle)*AllHelix[helix_index].thickness && 
	  min2(v1/v2,v2/v1) > max2(v1/v3,v2/v3)) {
	
	if (new_dx*dx+new_dy*dy+new_dz*dz < 0) {
	  new_dx = -new_dx;
	  new_dy = -new_dy;
	  new_dz = -new_dz;
	}
	AllHelix[helix_index].dx += new_dx;
	AllHelix[helix_index].dy += new_dy;
	AllHelix[helix_index].dz += new_dz;
	AllHelix[helix_index].thickness = (AllHelix[helix_index].thickness*
	    AllHelix[helix_index].size+(v1+v2)*0.5)/(float)(AllHelix[helix_index].size+1);
	AllHelix[helix_index].size += 1;
	pnt_obj = (PNTS*)malloc(sizeof(PNTS));
	pnt_obj->x = tx;
	pnt_obj->y = ty;
	pnt_obj->z = tz;
	pnt_obj->next = NULL;
	pnt_tmp = AllHelix[helix_index].points;
	while (pnt_tmp->next != NULL) 
	  pnt_tmp = pnt_tmp->next;
	pnt_tmp->next = pnt_obj;
	
	ax = tx;
	ay = ty;
	az = tz;
	dx = new_dx;
	dy = new_dy;
	dz = new_dz;
      }
      else 
	break;
    }
    else
      break;
  }


  dx = -start_dx;
  dy = -start_dy;
  dz = -start_dz;
  pnt_obj = AllHelix[helix_index].points;
  ax = pnt_obj->x;
  ay = pnt_obj->y;
  az = pnt_obj->z;
  while (1) {
    tx = ax + 0.5*dx;
    ty = ay + 0.5*dy;
    tz = az + 0.5*dz;
    
    x00 = dataset[IndexVect((int)tx,(int)ty,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz)]*(tx-(int)tx);
    x01 = dataset[IndexVect((int)tx,(int)ty,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty,(int)tz+1)]*(tx-(int)tx);
    x10 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz)]*(tx-(int)tx);
    x11 = dataset[IndexVect((int)tx,(int)ty+1,(int)tz+1)]*((int)tx+1-tx)+
      dataset[IndexVect((int)tx+1,(int)ty+1,(int)tz+1)]*(tx-(int)tx);
    y0  = x00*((int)ty+1-ty) + x10*(ty-(int)ty);
    y1  = x01*((int)ty+1-ty) + x11*(ty-(int)ty);
    density = y0*((int)tz+1-tz) + y1*(tz-(int)tz);
    if (density > t_low) {
      EigenVector = GetEigenVector(tx, ty, tz);
      v1 = GetThickness(tx, ty, tz,EigenVector.x1,EigenVector.y1,EigenVector.z1);
      v1 += GetThickness(tx, ty, tz,-EigenVector.x1,-EigenVector.y1,-EigenVector.z1);
      v2 = GetThickness(tx, ty, tz,EigenVector.x2,EigenVector.y2,EigenVector.z2);
      v2 += GetThickness(tx, ty, tz,-EigenVector.x2,-EigenVector.y2,-EigenVector.z2);
      v3 = GetThickness(tx, ty, tz,EigenVector.x3,EigenVector.y3,EigenVector.z3);
      v3 += GetThickness(tx, ty, tz,-EigenVector.x3,-EigenVector.y3,-EigenVector.z3);
      v1 *= 0.5;
      v2 *= 0.5;
      v3 *= 0.5;
      
      new_dx = EigenVector.x3;
      new_dy = EigenVector.y3;
      new_dz = EigenVector.z3;
      dx = AllHelix[helix_index].dx;
      dy = AllHelix[helix_index].dy;
      dz = AllHelix[helix_index].dz;
      length = sqrt(dx*dx+dy*dy+dz*dz);
      dx /= length;
      dy /= length;
      dz /= length;
      angle = new_dx*dx+new_dy*dy+new_dz*dz;
      if (angle < 0) 
	angle = -angle;
      angle = 1.0-2.0*acos(angle)/PIE;
      angle = angle*angle*angle;
      if ((v1+v2)*0.5 > (1.0-0.9*angle)*AllHelix[helix_index].thickness && 
	  (v1+v2)*0.5 < (1.0+0.9*angle)*AllHelix[helix_index].thickness && 
	  min2(v1/v2,v2/v1) > max2(v1/v3,v2/v3)) {
	
	if (new_dx*dx+new_dy*dy+new_dz*dz < 0) {
	  new_dx = -new_dx;
	  new_dy = -new_dy;
	  new_dz = -new_dz;
	}
	AllHelix[helix_index].dx += new_dx;
	AllHelix[helix_index].dy += new_dy;
	AllHelix[helix_index].dz += new_dz;
	AllHelix[helix_index].thickness = (AllHelix[helix_index].thickness*
	    AllHelix[helix_index].size+(v1+v2)*0.5)/(float)(AllHelix[helix_index].size+1);
	AllHelix[helix_index].size += 1;
	pnt_obj = (PNTS*)malloc(sizeof(PNTS));
	pnt_obj->x = tx;
	pnt_obj->y = ty;
	pnt_obj->z = tz;
	pnt_obj->next = AllHelix[helix_index].points;
	AllHelix[helix_index].points = pnt_obj;
	
	ax = tx;
	ay = ty;
	az = tz;
	dx = -new_dx;
	dy = -new_dy;
	dz = -new_dz;
      }
      else
	break;
    }
    else
      break;
  }
  
}



void DrawLine(float sx, float sy, float sz, float ex, float ey, float ez, float radius)
{
  float x,y,z;
  float xx,yy,zz;
  float xxx,yyy,zzz;
  float a[3][3],b[3][3];
  float theta, phi;
  int m;
  

  theta = atan2(sy-ey,sx-ex);
  phi = atan2(sz-ez, sqrt((sx-ex)*(sx-ex)+(sy-ey)*(sy-ey)));
  
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

  
  xx = sqrt((sy-ey)*(sy-ey)+(sx-ex)*(sx-ex));
  if (xx == 0) {
    x = radius+ex;
    y = ey;
    z = ez;
  }
  else {
    x = radius*(ey-sy)/xx+ex;
    y = radius*(sx-ex)/xx+ey;
    z = ez;
  }
  
  vertex[v_num].x = x;
  vertex[v_num].y = y;
  vertex[v_num].z = z;
  vertex[v_num+1].x = x+sx-ex;
  vertex[v_num+1].y = y+sy-ey;
  vertex[v_num+1].z = z+sz-ez;
    
  
  x = x-ex;
  y = y-ey;
  z = z-ez;
  
  xx = a[0][0]*x+a[0][1]*y+a[0][2]*z;
  yy = a[1][0]*x+a[1][1]*y+a[1][2]*z;
  zz = a[2][0]*x+a[2][1]*y+a[2][2]*z;
  
  for (m = 1; m < PolyNum; m++) {
    x = cos(2*PIE*(float)(m)/(float)(PolyNum))*xx - 
      sin(2*PIE*(float)(m)/(float)(PolyNum))*yy;
    y = sin(2*PIE*(float)(m)/(float)(PolyNum))*xx + 
      cos(2*PIE*(float)(m)/(float)(PolyNum))*yy;
    z = zz;
    
    xxx = b[0][0]*x+b[0][1]*y+b[0][2]*z+ex;
    yyy = b[1][0]*x+b[1][1]*y+b[1][2]*z+ey;
    zzz = b[2][0]*x+b[2][1]*y+b[2][2]*z+ez;
    
    vertex[v_num+2*m].x = xxx;
    vertex[v_num+2*m].y = yyy;
    vertex[v_num+2*m].z = zzz;
    vertex[v_num+2*m+1].x = xxx+sx-ex;
    vertex[v_num+2*m+1].y = yyy+sy-ey;
    vertex[v_num+2*m+1].z = zzz+sz-ez;
    
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
  
  vertex[v_num+2*PolyNum].x = ex;
  vertex[v_num+2*PolyNum].y = ey;
  vertex[v_num+2*PolyNum].z = ez;
  vertex[v_num+2*PolyNum+1].x = sx;
  vertex[v_num+2*PolyNum+1].y = sy;
  vertex[v_num+2*PolyNum+1].z = sz;

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


