#ifndef GRAYSKELETONCPP_VOLUME_FORMAT_CONVERTER_H
#define GRAYSKELETONCPP_VOLUME_FORMAT_CONVERTER_H

#include <string>
#include "VolumeReaderRAW.h"
#include "VolumeReaderATOM.h"
#include "VolumeReaderTXT.h"
#include <SkeletonMaker/reader.h>
#include <SkeletonMaker/volume.h>


using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeFormatConverter {
		public:
			static Volume * LoadVolume(string inputFile);
			static void ConvertVolume(string inputFile, string inputFormat, string outputFile, string outputFormat, int sizeX, int sizeY, int sizeZ);
		};

		Volume * VolumeFormatConverter::LoadVolume(string inputFile) {
			int pos = inputFile.rfind(".") + 1;
			string inputFormat = inputFile.substr(pos, inputFile.length()-pos);

			Volume * vol = NULL;			
			if(stricmp(inputFormat.c_str(), "MRC") == 0) {
				vol = MRCReaderPicker::pick((char *)inputFile.c_str())->getVolume();
			} else if (stricmp(inputFormat.c_str(), "ATOM") == 0) {		
				vol = VolumeReaderATOM::LoadVolume(inputFile);
			} else if (stricmp(inputFormat.c_str(), "TXT") == 0) {		
				vol = VolumeReaderTXT::LoadVolume(inputFile);
			} else {
				printf("Input format [%s] not supported!\n", (char *)inputFormat.c_str());			
			}
			return vol;
		}

		void VolumeFormatConverter::ConvertVolume(string inputFile, string inputFormat, string outputFile, string outputFormat, int sizeX, int sizeY, int sizeZ) {
			Volume * vol = NULL;			
			if(stricmp(inputFormat.c_str(), "MRC") == 0) {
				vol = MRCReaderPicker::pick((char *)inputFile.c_str())->getVolume();
			} else if (stricmp(inputFormat.c_str(), "RAW8") == 0) {		
				vol = VolumeReaderRAW::LoadVolume8bit(inputFile, sizeX, sizeY, sizeZ, 1, 1, 1);
			} else if (stricmp(inputFormat.c_str(), "RAW16") == 0) {		
				vol = VolumeReaderRAW::LoadVolume16bit(inputFile, sizeX, sizeY, sizeZ, 1, 1, 1);
			} else if (stricmp(inputFormat.c_str(), "ATOM") == 0) {		
				vol = VolumeReaderATOM::LoadVolume(inputFile);
			} else if (stricmp(inputFormat.c_str(), "TXT") == 0) {		
				vol = VolumeReaderTXT::LoadVolume(inputFile);
			} else {
				printf("Input format [%s] not supported!\n", inputFormat.c_str());
				return;
			}
					
			if(stricmp(outputFormat.c_str(), "MRC") == 0) {
				vol->toMRCFile((char *)outputFile.c_str());
			} else if (stricmp(outputFormat.c_str(), "NB") == 0) {
				vol->toMathematicaFile((char *)outputFile.c_str());
			} else if (stricmp(outputFormat.c_str(), "OFF0") == 0) {
				vol->toOFFCells((char *)outputFile.c_str());
			} else if (stricmp(outputFormat.c_str(), "OFF") == 0) {
				vol->toOFFCells2((char *)outputFile.c_str());
			} else {
				printf("Output format %s not supported!\n", outputFormat);
			}
			delete vol;
			
		}
	}
}

#endif