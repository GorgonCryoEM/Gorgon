// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing an base graph edge



#ifndef FOUNDATION_GRAPH_EDGE_BASE_H
#define FOUNDATION_GRAPH_EDGE_BASE_H

namespace wustl_mm {
	namespace Foundation {

		template <class TTag> class GraphEdgeBase {
		public:			
			GraphEdgeBase();
			GraphEdgeBase(float weight, TTag tag = NULL);			
			float GetWeight();
			TTag GetTag();
			void SetWeight(float weight);
			void SetTag(TTag tag);
		private:
			float weight;
			TTag tag;
		};

		template <class TTag> GraphEdgeBase<TTag>::GraphEdgeBase() {
			this->weight = 0;
			this->tag = TTag();
		}

		template <class TTag> GraphEdgeBase<TTag>::GraphEdgeBase(float weight, TTag tag) {			
			this->weight = weight;
			this->tag = tag;
		}

		template <class TTag> float GraphEdgeBase<TTag>::GetWeight() {
			return weight;
		}

		template <class TTag> TTag GraphEdgeBase<TTag>::GetTag() {
			return tag;
		}

		template <class TTag> void GraphEdgeBase<TTag>::SetWeight(float weight) {
			this->weight = weight;
		}

		template <class TTag> void GraphEdgeBase<TTag>::SetTag(TTag tag) {
			this->tag = tag;
		}

	}
}

#endif
