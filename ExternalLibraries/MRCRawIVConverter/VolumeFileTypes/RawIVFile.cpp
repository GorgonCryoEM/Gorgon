// RawIVFile.cpp: implementation of the RawIVFile class.
//
//////////////////////////////////////////////////////////////////////

#include "RawIVFile.h"
#include "../SimpleVolumeData.h"
#include "ByteSwapping.h"
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

RawIVFile RawIVFile::ms_RawIVFileRepresentative;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RawIVFile::RawIVFile()
{

}

RawIVFile::~RawIVFile()
{

}

SimpleVolumeData* RawIVFile::loadFile(const string& fileName)
{
	SimpleVolumeData* simpleVolumeData = new SimpleVolumeData(128, 128, 128);

	FILE *fp=fopen(fileName.c_str(), "rb");
	unsigned int dims[3], numverts, numcells;
	float minExt[3], maxExt[3], orig[3], span[3];

	// check to make sure the file exists
	if (!fp) {
		printf("Error: could not open file\n");
		delete simpleVolumeData; simpleVolumeData = 0;
		return 0;
	}

	// read the header
	// 
	fread(minExt, 3, sizeof(float), fp);
	fread(maxExt, 3, sizeof(float), fp);
	fread(&numverts, 1, sizeof(unsigned int), fp);
	fread(&numcells, 1, sizeof(unsigned int), fp);
	fread(dims, 3, sizeof(unsigned int), fp);
	fread(orig, 3, sizeof(float), fp);
	fread(span, 3, sizeof(float), fp);
	if (isLittleEndian()) {
		swapByteOrder(minExt, 3);
		swapByteOrder(maxExt, 3);
		swapByteOrder(&numverts, 1);
		swapByteOrder(&numcells, 1);
		swapByteOrder(dims, 3);
		swapByteOrder(orig, 3);
		swapByteOrder(span, 3);
	}

	// find out how large the data is
	long dataStart = ftell(fp), dataSize;
	fseek(fp, 0, SEEK_END);
	dataSize = ftell(fp) - dataStart;
	fseek(fp, dataStart, SEEK_SET);

	// a small sanity check to make sure this file is valid
	if ((dataSize % ((long)dims[0]*dims[1]*dims[2])) != 0) {
		printf("Error: rawiv file header dimensions don't match file size");
		fclose(fp);
		return false;
	}
	
	// call some set...() functions
	//
	simpleVolumeData->setNumberOfVariables(1);
	simpleVolumeData->setDimensions(dims);
	simpleVolumeData->setMinExtent(minExt);
	simpleVolumeData->setMaxExtent(maxExt);

	// figure out the data type
	switch (dataSize / ((long)dims[0]*dims[1]*dims[2])) {
	case 1: simpleVolumeData->setType(0, SimpleVolumeData::UCHAR); break;
		case 2: simpleVolumeData->setType(0, SimpleVolumeData::USHORT); break;
		case 4: simpleVolumeData->setType(0, SimpleVolumeData::FLOAT); break;
		default: simpleVolumeData->setType(0, SimpleVolumeData::NO_TYPE); break;
	}

	// allocate space for the data
	unsigned char * data = new unsigned char [dataSize];

	// read the data
	fread(data, dataSize, 1, fp);
	// swap the byte order if needed
	if (isLittleEndian()) {
		switch(simpleVolumeData->getTypeSize(0)) {
			case 1:
				break;
			case 2:
				swapByteOrder((unsigned short *)data, dims[0]*dims[1]*dims[2]);
				break;
			case 4:
				swapByteOrder((float *)data, dims[0]*dims[1]*dims[2]);
				break;
			case 8:
				swapByteOrder((double *)data, dims[0]*dims[1]*dims[2]);
				break;
			default:
				break;
		}
	}

	simpleVolumeData->setData(0, data);
	// close the file

	fclose(fp);
	return simpleVolumeData;
}

bool RawIVFile::checkType(const string& fileName)
{
	return false;
}

bool RawIVFile::saveFile(SimpleVolumeData* simpleVolumeData, const string& fileName, unsigned int variable)
{
	// no data? bail.
	if (!simpleVolumeData->getData(variable))
		return false;

	// unsupported datatype for the requested variable? bail.
	if (simpleVolumeData->getType(variable) == SimpleVolumeData::ULONG || simpleVolumeData->getType(variable) == SimpleVolumeData::DOUBLE)
		return false;

	FILE *fp = fopen(fileName.c_str(), "wb");
	char header[68];
	
	// failed to open the file? bail.
	if (!fp)
		return false;

	// make the data big endian
	simpleVolumeData->makeVariablesBigEndian();

	// create the header
	simpleVolumeData->createRawIVHeader(header);

	// write the header
	fwrite(header, sizeof(header), 1, fp);


	// write the data
	fwrite(simpleVolumeData->getData(variable), simpleVolumeData->getDataSize(variable), 1, fp);

	// make the data native endian (poorly named function)
	simpleVolumeData->makeVariablesBigEndian();
	
	// clean up
	fclose(fp);
	
	return true;
}

VolumeFileType* RawIVFile::getRepresentative()
{
	return &ms_RawIVFileRepresentative;
}

