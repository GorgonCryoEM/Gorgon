// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for instantiating a set of vector classes, and implementing their converters

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/MathTools/Vector3D.h,v $
//   $Revision: 1.8 $
//   $Date: 2009/12/07 05:00:52 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: Vector3D.h,v $
//   Revision 1.8  2009/12/07 05:00:52  ssa1
//   Adding in Matrix functionality for Singular Value Decomposition
//
//   Revision 1.7  2008/10/14 14:59:33  ssa1
//   Adding in sketching mode for interactive skeletonization
//
//   Revision 1.6  2008/09/29 16:43:15  ssa1
//   Adding in CVS meta information
//


#ifndef MATHTOOLS_VECTOR3D_H
#define MATHTOOLS_VECTOR3D_H

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

		inline unsigned long long GetHashFromVector3DInt(Vector3DInt &v) {
			long long twoPowerTen = 1<<10;
			unsigned long long xVal = (unsigned long long)((long long)v.X() + twoPowerTen);
			unsigned long long yVal = (unsigned long long)((long long)v.Y() + twoPowerTen);
			unsigned long long zVal = (unsigned long long)((long long)v.Z() + twoPowerTen);
			unsigned long long hash = (((xVal << 20) + yVal) << 20) + zVal;
			return hash;
		}

		inline Vector3DInt GetVector3DIntFromHash(unsigned long long hash) {
			long long twoPowerTen = 1<<10;
			int x, y, z;

			z = (int)((hash % (1 << 20)) - twoPowerTen);
			hash = (hash >> 20);
			y = (int)((hash % (1 << 20)) - twoPowerTen);
			hash = (hash >> 20);
			x = (int)(hash - twoPowerTen);
			return Vector3DInt(x, y, z);
		}

	}
}

#endif