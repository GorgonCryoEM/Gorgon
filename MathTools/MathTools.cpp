// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy CPP file for the MATHTOOLS Module

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.5  2008/09/29 16:43:15  ssa1
//   Adding in CVS meta information
//

#ifndef MATHTOOLS_SWITCHBOARD_CPP
#define MATHTOOLS_SWITCHBOARD_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include "BasicDefines.h"
#include "Combinatorics.h"
#include "ComplexNumber.h"
#include "DataStructures.h"
#include "MathLib.h"
#include "MatlabWrapper.h"
#include "Vector3D.h"
#include "Matrix.h"

using namespace wustl_mm::MathTools;
int main()
{	
	MatrixDouble m = MatrixDouble(2, 4);
	m.SetValue(1,0,0);
	m.SetValue(2,0,1);
	m.SetValue(3,0,2);
	m.SetValue(4,0,3);
	m.SetValue(5,1,0);
	m.SetValue(6,1,1);
	m.SetValue(7,1,2);
	m.SetValue(8,1,3);
	m.Print(false); printf("\n");

	MatrixDouble u = MatrixDouble(2, 2);
	MatrixDouble w = MatrixDouble(2, 4);
	MatrixDouble v = MatrixDouble(4, 4);


	m.SingularValueDecomposition(u, w, v);

	u.Print(false); printf("\n");
	w.Print(false); printf("\n");
	v.Print(false); printf("\n");

	(u * w * v.ConjugateTranspose()).Print(false); 

	return 0;
} 

#endif