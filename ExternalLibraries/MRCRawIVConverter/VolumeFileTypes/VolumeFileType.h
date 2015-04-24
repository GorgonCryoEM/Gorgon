// VolumeFileType.h: interface for the VolumeFileType class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_VOLUME_FILE_TYPE_H
#define CCV_VOLUME_FILE_TYPE_H

#include <string>
using std::string;

class SimpleVolumeData;

class VolumeFileType  
{
public:
	VolumeFileType();
	virtual ~VolumeFileType();

	virtual SimpleVolumeData* loadFile(const string& fileName) = 0;
	virtual bool checkType(const string& fileName) = 0;
	virtual bool saveFile(SimpleVolumeData* simpleVolumeData, const string& fileName, unsigned int variable=0) = 0;

	virtual string extension() = 0;
	virtual string filter() = 0;

};

#endif 
