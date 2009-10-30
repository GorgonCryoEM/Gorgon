// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Stores information of a density volume

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.1  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//



#ifndef SKELETON_MAKER_VOLUME_DATA_H
#define SKELETON_MAKER_VOLUME_DATA_H

namespace wustl_mm {
	namespace SkeletonMaker {
		class VolumeData {
		public:
			VolumeData(int sizeX, int sizeY, int sizeZ);
			VolumeData(int sizeX, int sizeY, int sizeZ, float val);
			VolumeData(int sizeX, int sizeY, int sizeZ, int offsetX, int offsetY, int offsetZ, VolumeData * data);
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

		VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ) {
			InitializeVolumeData(sizeX, sizeY, sizeZ, 1, 1, 1, 0, 0, 0, true, 0);
		}

		VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, float val) {
			InitializeVolumeData(sizeX, sizeY, sizeZ, 1, 1, 1, 0, 0, 0, true, val);
		}

		VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, int offsetX, int offsetY, int offsetZ, VolumeData * data) {
			InitializeVolumeData(sizeX, sizeY, sizeZ, data->GetSpacingX(), data->GetSpacingY(), data->GetSpacingZ(), data->GetOriginX(), data->GetOriginY(), data->GetOriginZ(), false, 0);
			int ct = 0 ;
			for (int i = offsetX; i < sizeX + offsetX; i++) {
				for (int j = offsetY; j < sizeY + offsetY; j++ ) {
					for ( int k = offsetZ; k < sizeZ + offsetZ; k++) {
						this->data[ct] = data->GetDataAt(i, j, k);
						ct++;
					}
				}
			}
		}

		VolumeData::~VolumeData() {
			delete [] data;
		}

		void VolumeData::InitializeVolumeData(int sizeX, int sizeY, int sizeZ, float spacingX, float spacingY, float spacingZ, float originX, float originY, float originZ, bool initializeData, float val) {
			SetSize(sizeX, sizeY, sizeZ);
			SetSpacing(spacingX, spacingY, spacingZ);
			SetOrigin(originX, originY, originZ);
			int maxIndex = GetMaxIndex();
			data = new float [maxIndex];
			if(initializeData) {
				for (int i=0; i < maxIndex; i++) {
					data[i] = val;
				}
			}
		}

		int VolumeData::GetSize(int dimension) {
			return size[dimension];
		}

		int VolumeData::GetSizeX() {
			return GetSize(0);
		}

		int VolumeData::GetSizeY() {
			return GetSize(1);
		}

		int VolumeData::GetSizeZ() {
			return GetSize(2);
		}

		float VolumeData::GetSpacing(int dimension) {
			return spacing[dimension];
		}

		float VolumeData::GetSpacingX() {
			return GetSpacing(0);
		}

		float VolumeData::GetSpacingY() {
			return GetSpacing(1);
		}

		float VolumeData::GetSpacingZ() {
			return GetSpacing(2);
		}

		float VolumeData::GetOrigin(int dimension) {
			return origin[dimension];
		}

		float VolumeData::GetOriginX() {
			return GetOrigin(0);
		}

		float VolumeData::GetOriginY() {
			return GetOrigin(1);
		}

		float VolumeData::GetOriginZ() {
			return GetOrigin(2);
		}


		float VolumeData::GetDataAt(int x, int y, int z) {
			return GetDataAt(GetIndex(x, y, z));
		}

		float VolumeData::GetDataAt(int index) {
			return data[index];
		}

		int VolumeData::GetIndex(int x, int y, int z) {
			return (x * GetSizeY() * GetSizeZ() + y * GetSizeZ() + z);
		}

		int VolumeData::GetMaxIndex() {
			return size[0] * size[1] * size[2];
		}

		void VolumeData::SetSpacing(float spacingX, float spacingY, float spacingZ) {
			spacing[0] = spacingX;
			spacing[1] = spacingY;
			spacing[2] = spacingZ;
		}

		void VolumeData::SetOrigin(float originX, float originY, float originZ) {
			origin[0] = originX;
			origin[1] = originY;
			origin[2] = originZ;
		}


		void VolumeData::SetSize(int sizeX, int sizeY, int sizeZ) {
			size[0] = sizeX;
			size[1] = sizeY;
			size[2] = sizeZ;
		}

		void VolumeData::SetDataAt(int x, int y, int z, float value) {
			SetDataAt(GetIndex(x, y, z), value);
		}

		void VolumeData::SetDataAt(int index, float value) {
			data[index] = value;
		}
		void VolumeData::Pad(int padBy, double padValue) {
			int sizex = GetSizeX();
			int sizey = GetSizeY();
			int sizez = GetSizeZ();
			int newSizeX = sizex + 2*padBy;
			int newSizeY = sizey + 2*padBy;
			int newSizeZ = sizez + 2*padBy;

			float * newData = new float[newSizeX * newSizeY * newSizeZ];
			double value;


			for(int x = 0; x < newSizeX; x++) {
				for(int y = 0; y < newSizeY; y++) {
					for(int z = 0; z < newSizeZ; z++) {
						if ((x < padBy) || (y < padBy) || (z < padBy) || (x >= padBy + sizex) || (y >= padBy + sizey) || (z >= padBy + sizez)) {
							value = padValue;
						} else {
							value = GetDataAt(x-padBy, y-padBy, z-padBy);
						}

						newData[x * newSizeY * newSizeZ + y * newSizeZ + z] = (float)value;
					}
				}
			}
			delete [] data;
			data = newData;
			SetSize(newSizeX, newSizeY, newSizeZ);

		}
	}
}


#endif
