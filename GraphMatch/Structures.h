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