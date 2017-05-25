// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for a face stored in a triangle mesh

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/ProteinMorph/TriangleMeshFace.h,v $
//   $Revision: 1.2 $
//   $Date: 2009/03/16 16:17:34 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: TriangleMeshFace.h,v $
//   Revision 1.2  2009/03/16 16:17:34  ssa1
//   Fitting SSEs into the Density
//


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