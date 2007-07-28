#ifndef SKELETAL_CURVE_H
#define SKELETAL_CURVE_H

#include <vector>
#include "..\MatlabInterface\DataStructures.h"

using namespace std;
using namespace wustl_mm::MatlabInterface;

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
