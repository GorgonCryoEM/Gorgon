// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for instantiating a set of matrix classes, and implementing their converters

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.7  2008/10/14 14:59:33  ssa1
//   Adding in sketching mode for interactive skeletonization
//
//   Revision 1.6  2008/09/29 16:43:15  ssa1
//   Adding in CVS meta information
//


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