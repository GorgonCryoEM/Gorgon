// DXFile.cpp: implementation of the DXFile class.
//
//////////////////////////////////////////////////////////////////////

#include "DXFile.h"
#include "../SimpleVolumeData.h"
#include "ByteSwapping.h"
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

DXFile DXFile::ms_DXFileRepresentative;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DXFile::DXFile()
{

}

DXFile::~DXFile()
{

}

bool DXFile::isCommentOrEmpty( const char* line )
{
	if( !line ) return true;
	if( strlen(line) < 1 ) return true;
	if( line[0] == '#' ) return true;

	return false;
}

void DXFile::tryToGetDimensions( bool* dimsSet, unsigned int* dims, const char* line )
{
	if( !dimsSet || !dims || !line ) return;

	//On reading "object 'A' class gridpositions counts 'B' 'C' 'D'" :
	// set dims to 'B', 'C', 'D'
	int dummy;
	if( sscanf( line, "object %d class gridpositions counts %d %d %d", 
		&dummy, 
		&(dims[2]), 
		&(dims[1]),
		&(dims[0])) != EOF ) //IMP note the order!
	{
		*dimsSet = true;
	}
}

void DXFile::tryToGetOrigin( bool* originSet, float* origin, const char* line )
{
	if( !originSet || !origin || !line ) return;
	double x=0, y=0, z=0;
	//origin -5.198200e+01 -5.250300e+01 -5.418250e+01
	if( sscanf( line, "origin %lf %lf %lf", &x, &y, &z ) != EOF )
	{
		*originSet = true;//IMP note the order!
		origin[2] = x;
		origin[1] = y;
		origin[0] = z;
	}
}

void DXFile::tryToGetSpans( int* deltasFound, float* span, const char* line )
{
	if( !deltasFound || !span || !line ) return;
	
	double deltas[3];
	/*delta 7.812500e-01 0.000000e+00 0.000000e+00
	  delta 0.000000e+00 7.812500e-01 0.000000e+00
	  delta 0.000000e+00 0.000000e+00 7.812500e-01*/
	if( sscanf( line, "delta %lf %lf %lf", &(deltas[0]), &(deltas[1]), &(deltas[2]) ) != EOF )
	{
		span[*deltasFound] = deltas[*deltasFound];
		(*deltasFound)++;
	}
}

void DXFile::tryToGetDataHeader(bool* dataHeaderFound, unsigned int* dims, const char* line)
{
	if( !dataHeaderFound || !dims || !line ) return;
	//object 3 class array type double rank 0 items 2146689 data follows
	int objectId, rankId, size;
	char type[256];
	if( sscanf( line, "object %d class array type %s rank %d items %d data follows",
		&objectId, type, &rankId, &size ) != EOF )
	{
		if( size != dims[0]*dims[1]*dims[2] ) return;
		*dataHeaderFound = true;
	}
}

SimpleVolumeData* DXFile::loadFile(const string& fileName)
{

	FILE *fp=fopen(fileName.c_str(), "r");
	if( !fp ) return 0;
	char line[256];
	bool dimsSet = false;
	bool originSet = false;
	bool dataHeaderFound = false;
	unsigned int dims[3];
	float minExtent[3];
	float span[3];
	float maxExtent[3];
	int deltasFound = 0;

	SimpleVolumeData* simpleVolumeData = new SimpleVolumeData(128, 128, 128);

	while( fgets( line, 256, fp ) != 0 )
	{
		if( isCommentOrEmpty( line ) ) continue;
		if( !dimsSet )
		{		
			tryToGetDimensions( &dimsSet, dims, line );
			if( dimsSet )
				simpleVolumeData->setDimensions(dims);
			continue;
		}
		if( !originSet )
		{
			tryToGetOrigin( &originSet, minExtent, line );
			if( originSet )
				simpleVolumeData->setMinExtent(minExtent);
			continue;
		}
		if( deltasFound < 3 )
		{
			tryToGetSpans( &deltasFound, span, line );
			if( deltasFound == 3 )
			{
				// IMP: note the order of the spans in the dx file!
				maxExtent[0] = minExtent[0] + (span[2])*(dims[0]-1);
				maxExtent[1] = minExtent[1] + (span[1])*(dims[1]-1);
				maxExtent[2] = minExtent[2] + (span[0])*(dims[2]-1);
			}
			continue;
		}
		if( !dataHeaderFound )
		{
			tryToGetDataHeader(&dataHeaderFound, dims, line);
			if( dataHeaderFound ) break;
			continue;
		}
	}
	if( !dataHeaderFound )
	{
		delete simpleVolumeData; simpleVolumeData = 0; return 0;
	}

	simpleVolumeData->setNumberOfVariables(1);
	simpleVolumeData->setDimensions(dims);
	simpleVolumeData->setMinExtent(minExtent);
	simpleVolumeData->setMaxExtent(maxExtent);
	simpleVolumeData->setType(0, SimpleVolumeData::FLOAT);

	float* data = new float[dims[0]* dims[1]* dims[2]];

	{
		int i;
		for( i=0; i<dims[0]* dims[1]* dims[2]; i++ )
		{
			double d;
			if( fscanf( fp, "%lf", &d ) != 1 )
			{
				delete []data; delete simpleVolumeData; simpleVolumeData = 0; return 0;
			}
			data[i] = (float)d;
		}
	}
	simpleVolumeData->setData(0, data);
	
	// close the file
	fclose(fp);
	
	return simpleVolumeData;
}

bool DXFile::checkType(const string& fileName)
{
	return false;
}

bool DXFile::saveFile(SimpleVolumeData* simpleVolumeData, const string& fileName, unsigned int variable)
{
	// no data? bail.
	if (!simpleVolumeData->getData(variable))
		return false;

	// unsupported datatype for the requested variable? bail.
	if (simpleVolumeData->getType(variable) == SimpleVolumeData::ULONG || simpleVolumeData->getType(variable) == SimpleVolumeData::DOUBLE)
		return false;

	FILE *fp = fopen(fileName.c_str(), "w");
	char *header=NULL;
	unsigned int hsize=0;
	
	// failed to open the file? bail.
	if (!fp)
		return false;

	// write the header
	fprintf(fp, "# Data from TexMol\n");
	fprintf(fp, "object 1 class gridpositions counts %d %d %d\n", 
		simpleVolumeData->getDepth(),
		simpleVolumeData->getHeight(),
		simpleVolumeData->getWidth()
		);
	fprintf(fp, "origin %e %e %e\n", 
		simpleVolumeData->getMinZ(), 
		simpleVolumeData->getMinY(), 
		simpleVolumeData->getMinX()
		);
	fprintf(fp, "delta %e %e %e\n", simpleVolumeData->getSpanZ(), 0.0, 0.0);
	fprintf(fp, "delta %e %e %e\n", 0.0, simpleVolumeData->getSpanY(), 0.0);
	fprintf(fp, "delta %e %e %e\n", 0.0, 0.0, simpleVolumeData->getSpanX());
	fprintf(fp, "object 2 class gridconnections counts %d %d %d\n",
		simpleVolumeData->getDepth(),
		simpleVolumeData->getHeight(),
		simpleVolumeData->getWidth()
		);
	int dataIndex = 3;
	int size = simpleVolumeData->getWidth() * simpleVolumeData->getHeight() * simpleVolumeData->getDepth();
	for (unsigned int i=0; i < simpleVolumeData->getNumberOfVariables(); i++)
	{
		// SKVINAY I dont know how to represent multiple volumes in teh same 
		// dx file. "Someone" correct this.

		fprintf(fp, "object %d class array type double rank 0 items %d data follows\n", 
				dataIndex,
				size
				);
		dataIndex++;
		// write the data
		{
			
			switch(simpleVolumeData->getType(i))
			{
			case SimpleVolumeData::UCHAR:
				{
					unsigned char* d = (unsigned char*)simpleVolumeData->getData(i);
					int c;
					for( c=0; c<size; c++ )
					{
						if( c>0 && (c % 3 == 0)) fprintf(fp, "\n");
						fprintf(fp, "%e ", (double)(d[c]));
					}
				}
				break;
			case SimpleVolumeData::USHORT:
				{
					unsigned short* d = (unsigned short*)simpleVolumeData->getData(i);
					int c;
					for( c=0; c<size; c++ )
					{
						if( c>0 && (c % 3 == 0)) fprintf(fp, "\n");
						fprintf(fp, "%e ", (double)(d[c]));
					}
				}
				break;
			case SimpleVolumeData::ULONG:
				{
					unsigned long* d = (unsigned long*)simpleVolumeData->getData(i);
					int c;
					for( c=0; c<size; c++ )
					{
						if( c>0 && (c % 3 == 0)) fprintf(fp, "\n");
						fprintf(fp, "%e ", (double)(d[c]));
					}
				}
				break;
			case SimpleVolumeData::FLOAT:
				{
					float* d = (float*)simpleVolumeData->getData(i);
					int c;
					for( c=0; c<size; c++ )
					{
						if( c>0 && (c % 3 == 0)) fprintf(fp, "\n");
						if( d[c] > 1.4 )
							fprintf(fp, "1.000000e+00 ");
						else
							fprintf(fp, "7.840000e+01 ");
						//fprintf(fp, "%e ", (double)(d[c]));
					}
				}
				break;
			case SimpleVolumeData::DOUBLE:
				{
					double* d = (double*)simpleVolumeData->getData(i);
					int c;
					for( c=0; c<size; c++ )
					{
						if( c>0 && (c % 3 == 0)) fprintf(fp, "\n");
						fprintf(fp, "%e ", (double)(d[c]));
					}
				}
				break;
			default:
				fclose(fp);
				return false;
			}
		}
	}
	
	fprintf(fp, "\n");

	fprintf(fp, "attribute \"dep\" string \"positions\"\n");
	fprintf(fp, "object \"regular positions regular connections\" class field\n");
	fprintf(fp, "component \"positions\" value 1\n");
	fprintf(fp, "component \"connections\" value 2\n");
	fprintf(fp, "component \"data\" value 3\n");

	// clean up
	delete [] header;
	fclose(fp);
	
	return true;
}

VolumeFileType* DXFile::getRepresentative()
{
	return &ms_DXFileRepresentative;
}

