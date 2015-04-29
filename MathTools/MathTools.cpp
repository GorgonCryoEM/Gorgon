// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy CPP file for the MATHTOOLS Module


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

	MatrixDouble n = MatrixDouble(2, 4);
	n.SetValue(1,0,0);
	n.SetValue(5,0,1);
	n.SetValue(6,0,2);
	n.SetValue(4,0,3);
	n.SetValue(2,1,0);
	n.SetValue(8,1,1);
	n.SetValue(6,1,2);
	n.SetValue(3,1,3);
	n.Print(false); printf("\n");

	MatrixDouble::Covariance(m, n).Print(false);

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
