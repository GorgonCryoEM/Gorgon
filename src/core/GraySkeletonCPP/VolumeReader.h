#ifndef CORE_GRAYSKELETONCPP_VOLUME_READER_H
#define CORE_GRAYSKELETONCPP_VOLUME_READER_H

#include <SkeletonMaker/volume.h>

using namespace SkeletonMaker;

namespace GraySkeletonCPP {
    class VolumeReader {
    public:
        static Volume * LoadVolume(string fileName);
        static void SaveVolume(Volume * volume, string fileName);
    };

    Volume * VolumeReader::LoadVolume(string fileName) {
        printf("VolumeReader::LoadVolume not implemented");
        return NULL;
    }


    void VolumeReader::SaveVolume(Volume * volume, string fileName) {
        printf("VolumeReader::SaveVolume not implemented");
    }
}

#endif
