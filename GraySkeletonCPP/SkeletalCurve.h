// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A data storage to store skeletal curves

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraySkeletonCPP/SkeletalCurve.h,v $
//   $Revision: 1.5 $
//   $Date: 2008/09/29 16:30:15 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: SkeletalCurve.h,v $
//   Revision 1.5  2008/09/29 16:30:15  ssa1
//   Adding in CVS meta information
//

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
