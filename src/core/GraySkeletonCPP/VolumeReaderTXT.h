#ifndef CORE_GRAYSKELETONCPP_VOLUME_READER_TXT_H
#define CORE_GRAYSKELETONCPP_VOLUME_READER_TXT_H

#include "VolumeReader.h"

using namespace SkeletonMaker;

namespace GraySkeletonCPP {

    class VolumeReaderTXT : VolumeReader {
    public:
        static Volume * LoadVolume(string fileName);
        static Volume * SaveVolume(Volume * volume, string fileName);
    };

    Volume * VolumeReaderTXT::LoadVolume(string fileName) {
        FILE * inFile = fopen(fileName.c_str(), "rt");
        int xSize, ySize, zSize, value;
        fscanf(inFile, "%d %d %d\n", &xSize, &ySize, &zSize);
        Volume * vol = new Volume(xSize,ySize,zSize);
        for(int z = 0; z < zSize; z++) {
            for(int y = 0; y < ySize; y++) {
                for(int x = 0; x < xSize; x++) {
                    fscanf(inFile, "%d ", &value);
                    vol->setDataAt(x, y, z, value);
                }
            }
        }

        fclose(inFile);
        return vol;
    }

}

#endif
