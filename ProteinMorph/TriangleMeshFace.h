#ifndef PROTEINMORPH_TRIANGLE_MESH_FACE_H
#define PROTEINMORPH_TRIANGLE_MESH_FACE_H

using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		template <class TFace> class TriangleMeshFace {
		public:
			unsigned long long vertexHashes[3];
			TFace tag;
		};
	}
}

#endif