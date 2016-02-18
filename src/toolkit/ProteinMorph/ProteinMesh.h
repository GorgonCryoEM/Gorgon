#ifndef PROTEINMORPH_PROTEIN_MESH_H
#define PROTEINMORPH_PROTEIN_MESH_H

#include "NonManifoldMesh.h"
#include <vector>

namespace wustl_mm {
	namespace Protein_Morph {
		struct Sheet {
			vector<int> faceIndices;
		};

		class ProteinMesh : public NonManifoldMesh {
		public:
			ProteinMesh();
			ProteinMesh(Volume * sourceVol, Volume * helixVol = NULL, Volume * sheetVol = NULL);
			void ToSSKFormat(string fileName);
		};

		ProteinMesh::ProteinMesh():NonManifoldMesh() {
		}

		ProteinMesh::ProteinMesh(Volume * sourceVol, Volume * helixVol, Volume * sheetVol):NonManifoldMesh(sourceVol, helixVol, sheetVol) {
		}

		
		void ProteinMesh::ToSSKFormat(string fileName) {
			FILE * outFile = fopen(fileName.c_str(), "wt");
			fprintf(outFile, "SSK\n");
			//vector<Sheet> = getSheets(
			//fprintf(outFile, "Sheets %i\n", sheetCount);

			fclose(outFile);

		}
	}
}

#endif;