// VolumeLoader.h: interface for the VolumeLoader class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_VOLUME_LOADER_H
#define CCV_VOLUME_LOADER_H

#include <string>
#include <map>
using std::string;

class VolumeFileType;
class SimpleVolumeData;

class VolumeLoader  
{
public:
	VolumeLoader();
	virtual ~VolumeLoader();

	bool saveFile(const string& fileName, const string& selectedFilter, SimpleVolumeData* simpleVolumeData, unsigned int variable=0);
	SimpleVolumeData* loadFile(const string& fileName);
	bool saveFile(const string& fileName, SimpleVolumeData* simpleVolumeData);

	string getLoadFilterString();
	string getSaveFilterString();

protected:
	string getAllExtensions();
	bool endsWith(string str, string substr);
	SimpleVolumeData* tryAll(const string& fileName);
	void addVolumeFileType(VolumeFileType* type);
	std::map<string, VolumeFileType*> m_ExtensionMap;
	std::map<string, VolumeFileType*> m_FilterMap;
};

#endif 
