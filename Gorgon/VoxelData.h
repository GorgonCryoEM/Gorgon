#ifndef _VOXEL_DATA_H_
#define _VOXEL_DATA_H_

#include <GraphMatch/VectorMath.h>
#include <string>
#include <SkeletonMaker/volume.h>
#include <GraySkeletonCPP/VolumeFormatConverter.h>

using namespace std;
using namespace wustl_mm::GraphMatch;
using namespace wustl_mm::GraySkeletonCPP;

namespace wustl_mm {
	namespace Visualization {
		class VoxelData {
		public:
			VoxelData();
			~VoxelData();
			void loadVolume(string filename);
			float getValue(float x, float y, float z);
			bool IsLoaded() const;

		public:
			Volume * data;
			
		};

		VoxelData::VoxelData() {
			data = NULL;
		}

		VoxelData::~VoxelData() {
			if(data != NULL) {
				delete data;
			}
		}

		void VoxelData::loadVolume(string filename) {
			if(data != NULL) {
				delete data;
			}
			data = VolumeFormatConverter::LoadVolume(filename);

		}

		float VoxelData::getValue(float x, float y, float z) {
			if(data != NULL) {
				x = min(max(x, 0.0f), 1.0f);
				y = min(max(y, 0.0f), 1.0f);
				z = min(max(z, 0.0f), 1.0f);
				int xPos = (int)(x * data->getSizeX())-1;
				int yPos = (int)(y * data->getSizeY())-1;
				int zPos = (int)(z * data->getSizeZ())-1;

				return data->getDataAt(xPos, yPos, zPos);
			} else {
				return 0;
			}
		}

		bool VoxelData::IsLoaded() const {
			return data != NULL;
		}

	}
}
#endif