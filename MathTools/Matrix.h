// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for instantiating a set of matrix classes, and implementing their converters



#ifndef MATHTOOLS_MATRIX_H
#define MATHTOOLS_MATRIX_H

#include "MatrixTemplate.h"

namespace wustl_mm {
	namespace MathTools {

		typedef MatrixTemplate<int> MatrixInt;
		typedef MatrixTemplate<float>  MatrixFloat;
		typedef MatrixTemplate<double> MatrixDouble;
		typedef MatrixTemplate<char> MatrixChar;

	}
}

#endif
