// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A collection of data structures used in the GRAPHMATCH module

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraphMatch/Structures.h,v $
//   $Revision: 1.3 $
//   $Date: 2008/09/29 16:19:30 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: Structures.h,v $
//   Revision 1.3  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//

#ifndef STRUCTURES_H
#define STRUCTURES_H

namespace wustl_mm {
	namespace GraphMatch {

		struct EdgeMinCostEntry {
			unsigned long long bitmap;
			double cost;
			int noOfEdges;
		};
	}
}
#endif