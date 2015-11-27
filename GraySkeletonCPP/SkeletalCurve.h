// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A data storage to store skeletal curves


#ifndef GRAYSKELETONCPP_SKELETAL_CURVE_H
#define GRAYSKELETONCPP_SKELETAL_CURVE_H

#include <vector>
#include <MathTools/DataStructures.h>

using namespace std;
using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class SkeletalCurve{
		public:
			SkeletalCurve();
			~SkeletalCurve();
		public:
			vector<Vector3DInt> points;
		};


		SkeletalCurve::SkeletalCurve() {
			points.clear();
		}


		SkeletalCurve::~SkeletalCurve() {
			points.clear();
		}


	}
}

#endif
