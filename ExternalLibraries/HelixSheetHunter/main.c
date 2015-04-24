/**************************************
 * Zeyun Yu (zeyun@cs.utexas.edu)    *
 * Department of Computer Science    *
 * University of Texas at Austin     *
 **************************************/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
  float x;
  float y;
  float z;
}VECTOR;

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


#define IndexVect(i,j,k) ((k)*XDIM*YDIM + (j)*XDIM + (i))
#define MAX_STRING    256

void Diffuse(float *, int, int, int);
void GVF_Compute(int, int, int, float *, VECTOR*);
void Canny(int, int, int, float, float, float*);
void StructureTensor(int, int, int, VECTOR*,unsigned char*,float*,float*,
		     float,float,float,float,float,float,int*);
void read_data(int *, int *, int *, float **, float *, float *, char *);
void write_rawiv_char(unsigned char*, FILE*);
void write_rawiv_short(unsigned short*, FILE*);
void write_rawiv_float(float*, FILE*);
void HelixHunter(int,int,int,float*,float*,float *,unsigned char *,
		 VECTOR *,float,float,float,FILE*,SEEDS*, int);
void SheetHunter(int,int,int,float*,float*,float *,unsigned char *,VECTOR *,
		 float,float,float,float,float,float,float,float,float,FILE*,SEEDS*,int,FILE*,FILE*);


int main(int argc, char *argv[])
{
  char file_name[MAX_STRING];
  VECTOR* velocity;
  unsigned char *result;
  float tlow;
  float helixwidth,min_hratio,max_hratio,min_hlength;
  float sheetwidth,min_sratio,max_sratio,sheet_extend;
  int XDIM, YDIM, ZDIM;
  float *dataset;
  float *span_tmp, *orig_tmp;
  FILE *fp,*fp1,*fp2;
  int i;
  time_t t1,t2;
  SEEDS *AllHelix_list; 
  int HSeedNum;
  

  if (argc != 10 && argc != 11){
    printf("Usage: HelixSheetHunter <input> <helixwidth> <min_helixwidth_ratio> <max_helixwidth_ratio> <min_helixlength> \n");
    printf("                        <sheetwidth> <min_sheetwidth_ratio> <max_sheetwidth_ratio> <sheet_extend> [threshold] \n");
    printf("       <input>: Rawiv file \n");
    printf("       <helixwidth> : the thickness of typical helices (in pixels) \n");
    printf("       <min_helixwidth_ratio> : the low ratio of thickness (0~1) \n");
    printf("       <max_helixwidth_ratio> : the high fofthickness (> 1) \n");
    printf("       <min_helixlength> : the shortest helices (in pixels) \n");
    printf("       <sheetwidth> : the thickness of typical sheets (in pixels) \n");
    printf("       <min_sheetwidth_ratio> : the low ratio of thickness (0~1) \n");
    printf("       <max_sheetwidth_ratio> : the high ratio of thickness (> 1) \n");
    printf("       <sheet_extend> : the extension ratio of sheets (1~2) \n");
    printf("       <threshold> : optional (0~255) \n");
    exit(0);              
  }

  (void)time(&t1); 
  
  printf("Loading dataset... \n");
  span_tmp = (float *)malloc(sizeof(float)*3);
  orig_tmp = (float *)malloc(sizeof(float)*3);
  read_data(&XDIM,&YDIM,&ZDIM,&dataset,span_tmp,orig_tmp,argv[1]);
  printf("Dataset loaded\n");
  
  
  
  span_tmp[0] = span_tmp[0]*(XDIM-1)/(float)XDIM;
  span_tmp[1] = span_tmp[1]*(YDIM-1)/(float)YDIM;
  span_tmp[2] = span_tmp[2]*(ZDIM-1)/(float)ZDIM;
  
  
  /*
  printf("Begin Diffusion....\n");
  Diffuse(dataset,XDIM,YDIM,ZDIM);
  */
  
  velocity = (VECTOR*)malloc(sizeof(VECTOR)*XDIM*YDIM*ZDIM);
  printf("Begin GVF computation....\n");
  GVF_Compute(XDIM,YDIM,ZDIM,dataset,velocity);

  result = (unsigned char*)malloc(sizeof(unsigned char)*XDIM*YDIM*ZDIM);
  printf("begin detecting features ....\n");
  helixwidth = atof(argv[2]);
  min_hratio = atof(argv[3]);
  max_hratio = atof(argv[4]);
  min_hlength = atof(argv[5]);
  sheetwidth = atof(argv[6]);
  min_sratio = atof(argv[7]);
  max_sratio = atof(argv[8]);
  sheet_extend = atof(argv[9]);
  if (argc == 10)
    tlow = -1; 
  else
    tlow = atof(argv[10]);

  StructureTensor(XDIM,YDIM,ZDIM,velocity,result,dataset,&tlow,helixwidth,min_hratio,
		  max_hratio, sheetwidth,min_sratio,max_sratio, &HSeedNum);
  (void)time(&t2); 
  printf("time used for preprocessing: %d seconds. \n\n",(int)(t2-t1));
  

  (void)time(&t1); 
  strcpy(file_name,argv[1]);
  for(i = 0; i<MAX_STRING; i++) {
    if (file_name[i] == '.' && file_name[i+1] == 'r' &&
	file_name[i+2] == 'a' && file_name[i+3] == 'w')
      break;
  }
  file_name[i+13] = file_name[i+6];
  file_name[i] = '_';
  file_name[i+1] = 's';
  file_name[i+2] = 'k';
  file_name[i+3] = 'e';
  file_name[i+4] = 'l';
  file_name[i+5] = 'e';
  file_name[i+6] = 't';
  file_name[i+7] = 'o';
  file_name[i+8] = 'n';
  file_name[i+9] = '.';
  file_name[i+10] = 'w';
  file_name[i+11] = 'r';
  file_name[i+12] = 'l';
  if ((fp=fopen(file_name, "w"))==NULL){
    printf("read error...\n");
    exit(0); 
  };    
  AllHelix_list = (SEEDS*)malloc(sizeof(SEEDS)*HSeedNum);
  HelixHunter(XDIM,YDIM,ZDIM,span_tmp,orig_tmp, dataset, result, 
	      velocity,tlow,helixwidth,min_hlength,fp,AllHelix_list,HSeedNum);
  
  (void)time(&t2); 
  printf("time used for helix-hunter: %d seconds. \n\n",(int)(t2-t1));
  
  (void)time(&t1); 
  strcpy(file_name,argv[1]);
  for(i = 0; i<MAX_STRING; i++) {
    if (file_name[i] == '.' && file_name[i+1] == 'r' &&
	file_name[i+2] == 'a' && file_name[i+3] == 'w')
      break;
  }
  file_name[i+10] = file_name[i+6];
  file_name[i] = '_';
  file_name[i+1] = 's';
  file_name[i+2] = 'h';
  file_name[i+3] = 'e';
  file_name[i+4] = 'e';
  file_name[i+5] = 't';
  file_name[i+6] = '.';
  file_name[i+7] = 'w';
  file_name[i+8] = 'r';
  file_name[i+9] = 'l';
  if ((fp=fopen(file_name, "w"))==NULL){
    printf("read error...\n");
    exit(0); 
  };    
  strcpy(file_name,argv[1]);
  for(i = 0; i<MAX_STRING; i++) {
    if (file_name[i] == '.' && file_name[i+1] == 'r' &&
	file_name[i+2] == 'a' && file_name[i+3] == 'w')
      break;
  }
  file_name[i+12] = file_name[i+6];
  file_name[i] = '_';
  file_name[i+1] = 'h';
  file_name[i+2] = 'e';
  file_name[i+3] = 'l';
  file_name[i+4] = 'i';
  file_name[i+5] = 'x';
  file_name[i+6] = '_';
  file_name[i+7] = 's';
  file_name[i+8] = '.';
  file_name[i+9] = 'w';
  file_name[i+10] = 'r';
  file_name[i+11] = 'l';
  if ((fp1=fopen(file_name, "w"))==NULL){
    printf("read error...\n");
    exit(0); 
  };    
  strcpy(file_name,argv[1]);
  for(i = 0; i<MAX_STRING; i++) {
    if (file_name[i] == '.' && file_name[i+1] == 'r' &&
	file_name[i+2] == 'a' && file_name[i+3] == 'w')
      break;
  }
  file_name[i+12] = file_name[i+6];
  file_name[i] = '_';
  file_name[i+1] = 'h';
  file_name[i+2] = 'e';
  file_name[i+3] = 'l';
  file_name[i+4] = 'i';
  file_name[i+5] = 'x';
  file_name[i+6] = '_';
  file_name[i+7] = 'c';
  file_name[i+8] = '.';
  file_name[i+9] = 'w';
  file_name[i+10] = 'r';
  file_name[i+11] = 'l';
  if ((fp2=fopen(file_name, "w"))==NULL){
    printf("read error...\n");
    exit(0); 
  };  

  SheetHunter(XDIM,YDIM,ZDIM,span_tmp,orig_tmp, dataset, result, velocity, tlow,helixwidth,min_hratio,
	      max_hratio,min_hlength,sheetwidth,min_sratio,max_sratio,sheet_extend,fp,AllHelix_list,HSeedNum,fp1,fp2);
  
  (void)time(&t2); 
  printf("time used for sheet-hunter: %d seconds. \n\n",(int)(t2-t1));
  
 
  free(dataset);
  free(result);
  free(velocity);
  
  return(0);
}

