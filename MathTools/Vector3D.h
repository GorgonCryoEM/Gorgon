// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for instantiating a set of vector classes, and implementing their converters

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$


#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "Vector3DTemplate.h"

namespace wustl_mm {
	namespace MathTools {

		typedef Vector3DTemplate<int>	 Vector3DInt;
		typedef Vector3DTemplate<float>  Vector3DFloat;
		typedef Vector3DTemplate<double> Vector3DDouble;
		typedef Vector3DTemplate<char> Vector3DChar;

		inline Vector3DFloat Vector3DInt2Float(Vector3DInt & v) {
			return Vector3DFloat((float)v.X(), (float)v.Y(), (float)v.Z());
		}

		inline Vector3DDouble Vector3DInt2Double(Vector3DInt & v) {
			return Vector3DDouble((double)v.X(), (double)v.Y(), (double)v.Z());
		}

	}
}

#endif