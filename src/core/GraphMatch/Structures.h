// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A collection of data structures used in the GRAPHMATCH module


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
