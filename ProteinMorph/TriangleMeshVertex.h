// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for vertex stored in a triangle mesh

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/ProteinMorph/TriangleMeshVertex.h,v $
//   $Revision: 1.2 $
//   $Date: 2009/03/16 16:17:34 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: TriangleMeshVertex.h,v $
//   Revision 1.2  2009/03/16 16:17:34  ssa1
//   Fitting SSEs into the Density
//


#ifndef PROTEINMORPH_TRIANGLE_MESH_VERTEX_H
#define PROTEINMORPH_TRIANGLE_MESH_VERTEX_H

using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		template <class TVertex> class TriangleMeshVertex {
		public:
			TriangleMeshVertex();
			TriangleMeshVertex(Vector3DFloat position);
		public:
			Vector3DFloat position;
			vector<unsigned long long> faceHashes;
			TVertex tag;
		};		

		template <class TVertex> TriangleMeshVertex<TVertex>::TriangleMeshVertex() {
			position = Vector3DFloat(0, 0, 0);
		}

		template <class TVertex> TriangleMeshVertex<TVertex>::TriangleMeshVertex(Vector3DFloat position) {
			this->position = position;
		}
	}
}

#endif