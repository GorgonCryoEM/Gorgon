#ifndef VOLUME_FORMAT_CONVERTER_H
#define VOLUME_FORMAT_CONVERTER_H

#include <string>

using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeFormatConverter {
		public:
			static void ConvertVolume(string inputFile, string inputFormat, string outputFile, string outputFormat, int sizeX, int sizeY, int sizeZ);
		};

		void VolumeFormatConverter::ConvertVolume(string inputFile, string inputFormat, string outputFile, string outputFormat, int sizeX, int sizeY, int sizeZ) {
			Volume * vol = NULL;			
			if(stricmp(inputFormat.c_str(), "MRC") == 0) {
				vol = MRCReaderPicker::pick((char *)inputFile.c_str())->getVolume();
			} else if (stricmp(inputFormat.c_str(), "RAW") == 0) {		
				vol = VolumeReaderRAW::LoadVolume(inputFile, sizeX, sizeY, sizeZ, 1, 1, 1);
			} else {
				printf("Input format %s not supported!\n", inputFormat);
				return;
			}
					
			if(stricmp(outputFormat.c_str(), "MRC") == 0) {
				vol->toMRCFile((char *)outputFile.c_str());
			} else if (stricmp(outputFormat.c_str(), "NB") == 0) {
				vol->toMathematicaFile((char *)outputFile.c_str());
			} else if (stricmp(outputFormat.c_str(), "OFF") == 0) {
				vol->toOFFCells((char *)outputFile.c_str());
			} else {
				printf("Output format %s not supported!\n", outputFormat);
			}
			delete vol;
			
		}
	}
}

#endif