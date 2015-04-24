// RawIVFile.h: interface for the RawIVFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_RAWIV_FILE_H
#define CCV_RAWIV_FILE_H

#include "VolumeFileType.h"

class RawIVFile : public VolumeFileType  
{
public:
	virtual ~RawIVFile();

	virtual SimpleVolumeData* loadFile(const string& fileName);
	virtual bool checkType(const string& fileName);
	virtual bool saveFile(SimpleVolumeData* simpleVolumeData, const string& fileName, unsigned int variable=0);

	virtual string extension() { return "rawiv"; };
	virtual string filter() { return "RawIV files (*.rawiv)"; };

	static RawIVFile ms_RawIVFileRepresentative;
	static VolumeFileType* getRepresentative();

protected:
	RawIVFile();
};

#endif 
