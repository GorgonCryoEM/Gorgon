#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "Vector3D.h"

namespace wustl_mm {
	namespace MathTools {
		const int MAX_PROBABILITY_DISTRIBUTION_RADIUS = 10;
		const int MAX_PROBABILITY_DISTRIBUTION_WIDTH = MAX_PROBABILITY_DISTRIBUTION_RADIUS*2 + 1;

		struct EigenVectorsAndValues2D{
			double eigenVectors[2][2];
			double eigenValues[2];
			double structureTensor[2][2];
		};

		struct EigenVectorsAndValues3D{
			double eigenVectors[3][3];
			double eigenValues[3];
			double structureTensor[3][3];
		};

		struct ProbabilityDistribution1D{
			int radius;
			double values[MAX_PROBABILITY_DISTRIBUTION_WIDTH];
		};

		struct ProbabilityDistribution2D{
			int radius;
			double values[MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH];
		};

		struct ProbabilityDistribution3D{
			int radius;
			double values[MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH];
		};

		struct Quarternion {
			double values[4];
		};

		struct EigenResults3D{
			double values[3];
			Vector3D vectors[3];
		};

	}
}

#endif