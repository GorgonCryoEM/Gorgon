// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A correspondence feature



#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_FEATURE_H
#define PROTEINMORPH_SSE_CORRESPONDENCE_FEATURE_H

#include <MathTools/Vector3D.h>
#include <cassert>

using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Protein_Morph {
		class SSECorrespondenceFeature {
		public:
			SSECorrespondenceFeature(Vector3DFloat pt1, Vector3DFloat pt2);	
			Vector3DFloat GetEndPoint(int ix);
			Vector3DFloat GetCentroid();
			Vector3DFloat GetFeatureVector();
			float Length();
		private:
			Vector3DFloat endPoints[2];

		};

		SSECorrespondenceFeature::SSECorrespondenceFeature(Vector3DFloat pt1, Vector3DFloat pt2) {
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

		Vector3DFloat SSECorrespondenceFeature::GetEndPoint(int ix) {
			assert((ix >= 0) && (ix <= 1));
			return endPoints[ix];
		}

		float SSECorrespondenceFeature::Length() {
			return (endPoints[0] - endPoints[1]).Length();
		}

		Vector3DFloat SSECorrespondenceFeature::GetCentroid() {
			return (endPoints[0] + endPoints[1]) * 0.5;
		}

		Vector3DFloat SSECorrespondenceFeature::GetFeatureVector() {
			return (endPoints[1] - endPoints[0]);
		}

	}
}


#endif
