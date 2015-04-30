// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Stores data structures for the MATHTOOLS Module


#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "Vector3D.h"

namespace wustl_mm {
	namespace MathTools {
		const int MAX_PROBABILITY_DISTRIBUTION_RADIUS = 10;
		const int MAX_PROBABILITY_DISTRIBUTION_WIDTH = MAX_PROBABILITY_DISTRIBUTION_RADIUS*2 + 1;

		struct EigenVectorsAndValues2D{
			float eigenVectors[2][2];
			float eigenValues[2];
			float structureTensor[2][2];
		};

		struct EigenVectorsAndValues3D{
			float eigenVectors[3][3];
			float eigenValues[3];
			float structureTensor[3][3];
		};

		struct ProbabilityDistribution1D{
			int radius;
			float values[MAX_PROBABILITY_DISTRIBUTION_WIDTH];
		};

		struct ProbabilityDistribution2D{
			int radius;
			float values[MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH];
		};

		struct ProbabilityDistribution3D{
			int radius;
			float values[MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH][MAX_PROBABILITY_DISTRIBUTION_WIDTH];
		};

		struct Quarternion {
			float values[4];
		};

		struct EigenResults3D{
			float values[3];
			Vector3DFloat vectors[3];
		};

	}
}

#endif
