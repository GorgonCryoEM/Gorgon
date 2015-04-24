// MRCFile.h: interface for the MRCFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_MRCFILE_TYPE_H
#define CCV_MRCFILE_TYPE_H

#include "VolumeFileType.h"

typedef struct {
	
	int    nx;            // # of columns ( fastest changing in the map    
	int    ny;            // # of rows                                     
	
	int    nz;            // # of sections (slowest changing in the map    
	
	int    mode;          // data type
						  //      0 = image data in bytes
						  //      1 = image data in short integer
						  //      2 = image data in floats
						  //      3 = complex data in complex short integers
						  //      4 = complex data in complex reals          
	
	int    nxstart;       // number of first column in map (default = 0)   
	int    nystart;       // number of first row in map (default = 0)      
	int    nzstart;       // number of first ssection in map (default = 0) 
	
	int    mx;            // number of intervals along X                   
	int    my;            // number of intervals along Y                   
	int    mz;            // number of intervals along Z                   
	
	float  xlength;       // cell dimensions in X (angstrom)               
	float  ylength;       // cell dimensions in Y (angstrom)               
	float  zlength;       // cell dimensions in Z (angstrom)               
	
	float  alpha;         // cell angles between Y and Z                   
	float  beta;          // cell angles between X and Z                   
	float  gamma;         // cell angles between X and Y                   
	
	int    mapc;          // number of axis corresponding to columns (X)   
	int    mapr;          // number of axis corresponding to rows (Y)      
	int    maps;          // number of axis corresponding to sections (Z)  
	
	float  amin;          // minimum density value                         
	float  amax;          // maximum density value                         
	float  amean;         // mean density value                            
	
	int    ispg;          // space group number (0 for images)             
	int    nsymbt;        // # of bytes for symmetry operators             
	
	int    extra[29];     // user defined storage space                    
	
	float  xorigin;       // X phase origin                                
	float  yorigin;       // Y phase origin                                
	
	int    nlabl;         // # of labels being used in the MRC header      
	
	char   label[10][80]; // actual text labels                            
	
} OLDMrcHeader;

typedef struct {
	
	int    nx;            // # of columns ( fastest changing in the map    
	int    ny;            // # of rows                                     
	
	int    nz;            // # of sections (slowest changing in the map    
	
	int    mode;          // data type
						  //      0 = image data in bytes
						  //      1 = image data in short integer
						  //      2 = image data in floats
						  //      3 = complex data in complex short integers
						  //      4 = complex data in complex reals          
	
	int    nxstart;       // number of first column in map (default = 0)   
	int    nystart;       // number of first row in map (default = 0)      
	int    nzstart;       // number of first ssection in map (default = 0) 
	
	int    mx;            // number of intervals along X                   
	int    my;            // number of intervals along Y                   
	int    mz;            // number of intervals along Z                   
	
	float  xlength;       // cell dimensions in X (angstrom)               
	float  ylength;       // cell dimensions in Y (angstrom)               
	float  zlength;       // cell dimensions in Z (angstrom)               
	
	float  alpha;         // cell angles between Y and Z                   
	float  beta;          // cell angles between X and Z                   
	float  gamma;         // cell angles between X and Y                   
	
	int    mapc;          // number of axis corresponding to columns (X)   
	int    mapr;          // number of axis corresponding to rows (Y)      
	int    maps;          // number of axis corresponding to sections (Z)  
	
	float  amin;          // minimum density value                         
	float  amax;          // maximum density value                         
	float  amean;         // mean density value                            
	
	int    ispg;          // space group number (0 for images)             
	int    nsymbt;        // # of bytes for symmetry operators             
	
	int    extra[25];     // user defined storage space                    
	
	float  xorigin;       // X phase origin                                
	float  yorigin;       // Y phase origin                                
	float  zorigin;       // Z phase origin

	char   map[4];        // character string 'MAP '

	int    machst;        // machine stamp

	float  rms;           // rms deviation of map from mean density
	
	int    nlabl;         // # of labels being used in the MRC header      
	
	char   label[10][80]; // actual text labels                            
	
} MrcHeader;

class MRCFile : public VolumeFileType  
{
public:
	virtual ~MRCFile();

	virtual SimpleVolumeData* loadFile(const string& fileName);
	virtual bool checkType(const string& fileName);
	virtual bool saveFile(SimpleVolumeData* simpleVolumeData, const string& fileName, unsigned int variable=0);

	virtual string extension() { return "mrc"; };
	virtual string filter() { return "MRC files (*.mrc)"; };

	static MRCFile ms_MRCFileRepresentative;
	static VolumeFileType* getRepresentative();

protected:
	MRCFile();
	bool readHeader(FILE* fp, int fileSize, bool* swap, SimpleVolumeData* simpleVolumeData);
	bool checkHeader(MrcHeader& header, unsigned int fileSize);
	void swapHeader(MrcHeader& header);
	bool interpretOldHeader(MrcHeader& header, unsigned int fileSize, SimpleVolumeData* simpleVolumeData);
	bool interpretNewHeader(MrcHeader& header, unsigned int fileSize, SimpleVolumeData* simpleVolumeData);
	bool fillHeader(MrcHeader& header, SimpleVolumeData* simpleVolumeData);

};

#endif
