// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing an base graph vertex



#ifndef FOUNDATION_GRAPH_VERTEX_BASE_H
#define FOUNDATION_GRAPH_VERTEX_BASE_H

namespace wustl_mm {
	namespace Foundation {

		template <class TTag> class GraphVertexBase {
		public:			
			GraphVertexBase(unsigned long long externalIndex, float weight, TTag tag = NULL);			
			unsigned long long GetExternalIndex();
			float GetWeight();
			TTag GetTag();
			unsigned long long GetValence();
			unsigned long long GetEdge(unsigned int edgeIndex);
			void SetExternalIndex(unsigned long long externalIndex);
			void SetWeight(float weight);
			void SetTag(TTag tag);
			void AddEdge(unsigned long long edgeHash);
		private:
			vector<unsigned long long> edges;
			unsigned long long externalIndex;
			float weight;
			TTag tag;
		};

		template <class TTag> GraphVertexBase<TTag>::GraphVertexBase(unsigned long long externalIndex, float weight, TTag tag) {			
			this->externalIndex = externalIndex;
			this->weight = weight;
			this->tag = tag;
			edges.clear();
		}

		template <class TTag> unsigned long long GraphVertexBase<TTag>::GetExternalIndex() {
			return externalIndex;
		}

		template <class TTag> float GraphVertexBase<TTag>::GetWeight() {
			return weight;
		}

		template <class TTag> TTag GraphVertexBase<TTag>::GetTag() {
			return tag;
		}

		template <class TTag> unsigned long long GraphVertexBase<TTag>::GetValence() {
			return edges.size();
		}

		template <class TTag> void GraphVertexBase<TTag>::SetExternalIndex(unsigned long long externalIndex) {
			this->externalIndex = externalIndex;
		}

		template <class TTag> void GraphVertexBase<TTag>::SetWeight(float weight) {
			this->weight = weight;
		}

		template <class TTag> void GraphVertexBase<TTag>::SetTag(TTag tag) {
			this->tag = tag;
		}

		template <class TTag> void GraphVertexBase<TTag>::AddEdge(unsigned long long edgeHash) {
			edges.push_back(edgeHash);
		}

		template <class TTag> unsigned long long GraphVertexBase<TTag>::GetEdge(unsigned int edgeIndex) {
			return edges[edgeIndex];
		}



	}
}

#endif
