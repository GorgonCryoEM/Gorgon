#ifndef CORE_GRAYSKELETONCPP_VOLUME_FORMAT_CONVERTER_H
#define CORE_GRAYSKELETONCPP_VOLUME_FORMAT_CONVERTER_H

//#include <string>
#include <Foundation/StringUtils.h>
#include <SkeletonMaker/reader.h>
//#include <SkeletonMaker/volume.h>



//using namespace std;
using namespace Foundation;
using namespace SkeletonMaker;

namespace GraySkeletonCPP {
    class VolumeFormatConverter {
    public:
        static Volume * LoadVolume(string inputFile, int bitsPerCell = 8, int sizeX = 0, int sizeY = 0, int sizeZ = 0);
        static void ConvertVolume(string inputFile, string inputFormat, string outputFile, string outputFormat, int sizeX, int sizeY, int sizeZ);
    };

    Volume * VolumeFormatConverter::LoadVolume(string inputFile, int bitsPerCell, int sizeX, int sizeY, int sizeZ) {
        int pos = inputFile.rfind(".") + 1;
        string inputFormat = inputFile.substr(pos, inputFile.length()-pos);
        inputFormat = StringUtils::StringToUpper(inputFormat);

        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   VolumeFormatConverter.h"<<endl;
              cout<<"DEBUG: Method: VolumeFormatConverter::LoadVolume\033[0m"<<endl;
              cout<<"DEBUG: Args: string, int, int, int, int\033[0m"<<endl;
              cout<<"Filename: "
                  <<" "<<inputFile
                  <<endl
                  <<"extension: "
                  <<inputFormat
                  <<endl;
        #endif


        Volume * vol = NULL;
        if(strcmp(inputFormat.c_str(), "MRC") == 0) {
            vol = MRCReaderPicker::pick((char *)inputFile.c_str())->getVolume();
        } else {
            printf("Input format [%s] not supported!\n", (char *)inputFormat.c_str());
        }

        #ifdef GORGON_DEBUG
              cout<<"\033[35mDEBUG: File:   VolumeFormatConverter.h"<<endl;
              cout<<"DEBUG: Method: VolumeFormatConverter::LoadVolume\033[0m"<<endl;
              cout<<"DEBUG: Args: string, int, int, int, int\033[0m"<<endl;
              cout<<vol->getSize()<<endl;
        #endif

        return vol;
    }

    void VolumeFormatConverter::ConvertVolume(string inputFile, string inputFormat, string outputFile, string outputFormat, int sizeX, int sizeY, int sizeZ) {
        Volume * vol = NULL;
        inputFormat = StringUtils::StringToUpper(inputFormat);

        if(strcmp(inputFormat.c_str(), "MRC") == 0) {
            vol = MRCReaderPicker::pick((char *)inputFile.c_str())->getVolume();
        } else {
            printf("Input format [%s] not supported!\n", inputFormat.c_str());
            return;
        }

        outputFormat = StringUtils::StringToUpper(outputFormat);

        if(strcmp(outputFormat.c_str(), "MRC") == 0) {
            vol->toMRCFile((char *)outputFile.c_str());
        } else {
            printf("Output format %s not supported!\n", outputFormat.c_str());
        }
        delete vol;

    }
}

#endif
