// DXFile.h: interface for the DXFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_DX_FILE_H
#define CCV_DX_FILE_H

#include "VolumeFileType.h"

class DXFile : public VolumeFileType  
{
public:
	virtual ~DXFile();

	virtual SimpleVolumeData* loadFile(const string& fileName);
	virtual bool checkType(const string& fileName);
	virtual bool saveFile(SimpleVolumeData* simpleVolumeData, const string& fileName, unsigned int variable=0);

	virtual string extension() { return "dx"; };
	virtual string filter() { return "DX files (*.dx)"; };

	static DXFile ms_DXFileRepresentative;
	static VolumeFileType* getRepresentative();

protected:
	DXFile();
	bool isCommentOrEmpty( const char* line );
	void tryToGetDimensions( bool* dimsSet, unsigned int* dims, const char* line );
	void tryToGetOrigin( bool* originSet, float* origin, const char* line );
	void tryToGetSpans( int* deltasFound, float* span, const char* line );
	void tryToGetDataHeader(bool* dataHeaderFound, unsigned int* dims, const char* line);

};

#endif 
