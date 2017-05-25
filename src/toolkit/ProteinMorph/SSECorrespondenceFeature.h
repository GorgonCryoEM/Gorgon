// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A correspondence feature

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/ProteinMorph/SSECorrespondenceFeature.h,v $
//   $Revision: 1.1 $
//   $Date: 2009/08/26 14:58:55 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: SSECorrespondenceFeature.h,v $
//   Revision 1.1  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//


#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_FEATURE_H
#define PROTEINMORPH_SSE_CORRESPONDENCE_FEATURE_H

#include <MathTools/Vector3.h>
#include <cassert>
using namespace MathTools;

//using namespace wustl_mm::MathTools;

//namespace wustl_mm {
//	namespace Protein_Morph {
		class SSECorrespondenceFeature {
		public:
			SSECorrespondenceFeature(Vec3F pt1, Vec3F pt2);	
			Vec3F GetEndPoint(int ix);
			Vec3F GetCentroid();
			Vec3F GetFeatureVector();
			float Length();
		private:
			Vec3F endPoints[2];

		};

		inline SSECorrespondenceFeature::SSECorrespondenceFeature(Vec3F pt1, Vec3F pt2) {
			endPoints[0] = pt1;
			endPoints[1] = pt2;
			/*if(pt1 > pt2) {
				endPoints[0] = pt2;
				endPoints[1] = pt1;
			} else {
				endPoints[0] = pt1;
				endPoints[1] = pt2;
			}*/
		}

		inline Vec3F SSECorrespondenceFeature::GetEndPoint(int ix) {
			assert((ix >= 0) && (ix <= 1));
			return endPoints[ix];
		}

		inline float SSECorrespondenceFeature::Length() {
			return (endPoints[0] - endPoints[1]).length();
		}

		inline Vec3F SSECorrespondenceFeature::GetCentroid() {
			return (endPoints[0] + endPoints[1]) * 0.5;
		}

		inline Vec3F SSECorrespondenceFeature::GetFeatureVector() {
			return (endPoints[1] - endPoints[0]);
		}

//	}
//}


#endif
