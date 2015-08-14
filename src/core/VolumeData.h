/*
 * VolumeData.h
 *
 *  Created on: Aug 14, 2015
 *      Author: shadow_walker
 */

#ifndef SRC_CORE_VOLUMEDATA_H_
#define SRC_CORE_VOLUMEDATA_H_

#include <cstdlib>
using std::malloc;

class VolumeData {
public:
	VolumeData(int sizeX, int sizeY, int sizeZ);
	VolumeData(int sizeX, int sizeY, int sizeZ, float val);
	VolumeData(int sizeX, int sizeY, int sizeZ, int offsetX, int offsetY, int offsetZ, VolumeData * data);
	VolumeData(VolumeData& obj);
	~VolumeData();

	int GetSize(int dimension);
	int GetSizeX();
	int GetSizeY();
	int GetSizeZ();
	float GetSpacing(int dimension);
	float GetSpacingX();
	float GetSpacingY();
	float GetSpacingZ();
	float GetOrigin(int dimension);
	float GetOriginX();
	float GetOriginY();
	float GetOriginZ();
	float GetDataAt(int x, int y, int z);
	float GetDataAt(int index);
	int GetIndex(int x, int y, int z);
	int GetMaxIndex();
	float* GetArrayCopy(int padX=0, int padY=0, int padZ=0, float padValue=0); //uses malloc as required by FFT libraries

	void SetSpacing(float spacingX, float spacingY, float spacingZ);
	void SetOrigin(float originX, float originY, float originZ);
	void SetDataAt(int x, int y, int z, float value);
	void SetDataAt(int index, float value);
	void Pad(int padBy, double padValue);
private:
	void InitializeVolumeData(int sizeX, int sizeY, int sizeZ, float spacingX, float spacingY, float spacingZ, float originX, float originY, float originZ, bool initializeData, float val);
	void SetSize(int sizeX, int sizeY, int sizeZ);
private:
	int size[3];
	float spacing[3];
	float origin[3];
	float * data;
};

#endif /* SRC_CORE_VOLUMEDATA_H_ */
