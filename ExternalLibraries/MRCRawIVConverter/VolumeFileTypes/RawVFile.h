// RawVFile.h: interface for the RawVFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_RAWVFILE_H
#define CCV_RAWVFILE_H

#include "VolumeFileType.h"

class RawVFile : public VolumeFileType  
{
public:
	virtual ~RawVFile();

	virtual SimpleVolumeData* loadFile(const string& fileName);
	virtual bool checkType(const string& fileName);
	virtual bool saveFile(SimpleVolumeData* simpleVolumeData, const string& fileName, unsigned int variable=0);

	virtual string extension() { return "rawv"; };
	virtual string filter() { return "RawV files (*.rawv)"; };

	static RawVFile ms_RawVFileRepresentative;
	static VolumeFileType* getRepresentative();

protected:
	RawVFile();
};

#endif 
