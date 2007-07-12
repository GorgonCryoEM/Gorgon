#ifndef VOLUME_FORMAT_CONVERTER_H
#define VOLUME_FORMAT_CONVERTER_H

#include <string>
namespace wustl_mm {
	namespace GraySkeletonCPP {
		class VolumeFormatConverter {
		public:
			static void ConvertVolume(char * inputFile, char * inputFormat, char * outputFile, char * outputFormat, int sizeX, int sizeY, int sizeZ);
		};

		void VolumeFormatConverter::ConvertVolume(char * inputFile, char * inputFormat, char * outputFile, char * outputFormat, int sizeX, int sizeY, int sizeZ) {
			Volume * vol = NULL;
			if(strcmp(inputFormat, "MRC") == 0) {
				vol = MRCReaderPicker::pick(inputFile)->getVolume();
			} else if (strcmp(inputFormat, "RAW") == 0) {		
				vol = VolumeReaderRAW::LoadVolume(inputFile, sizeX, sizeY, sizeZ, 1, 1, 1);
			} else {
				printf("Input format %s not supported!\n", inputFormat);
				return;
			}
					
			if(strcmp(outputFormat, "MRC") == 0) {
				vol->toMRCFile(outputFile);
			} else if (strcmp(outputFormat, "NB") == 0) {
				vol->toMathematicaFile(outputFile);
			} else if (strcmp(outputFormat, "OFF") == 0) {
				vol->toOFFCells(outputFile);
			} else {
				printf("Output format %s not supported!\n", outputFormat);
			}
			delete vol;
			
		}
	}
}

#endif