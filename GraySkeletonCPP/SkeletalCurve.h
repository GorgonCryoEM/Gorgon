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
