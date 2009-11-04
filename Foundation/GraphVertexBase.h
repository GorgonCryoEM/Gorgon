// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing an base graph vertex

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.3  2009/09/02 19:06:13  ssa1
//   Working towards flexible fitting
//
//   Revision 1.2  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//
//   Revision 1.1  2009/08/18 19:55:06  ssa1
//   Adding A base graph class for finding maximal cliques, and connected components


#ifndef FOUNDATION_GRAPH_VERTEX_BASE_H
#define FOUNDATION_GRAPH_VERTEX_BASE_H

namespace wustl_mm {
	namespace Foundation {

		template <class TTag> class GraphVertexBase {
		public:			
			GraphVertexBase(unsigned long long index, float weight, TTag tag = null);			
			unsigned long long GetIndex();
			float GetWeight();
			TTag GetTag();
			unsigned long long GetValence();
			unsigned long long GetEdge(unsigned int edgeIndex);
			void SetIndex(unsigned long long index);
			void SetWeight(float weight);
			void SetTag(TTag tag);
			void AddEdge(unsigned long long edgeHash);
		private:
			vector<unsigned long long> edges;
			unsigned long long index;
			float weight;
			TTag tag;
		};

		template <class TTag> GraphVertexBase<TTag>::GraphVertexBase(unsigned long long index, float weight, TTag tag) {			
			this->index = index;
			this->weight = weight;
			this->tag = tag;
			edges.clear();
		}

		template <class TTag> unsigned long long GraphVertexBase<TTag>::GetIndex() {
			return index;
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

		template <class TTag> void GraphVertexBase<TTag>::SetIndex(unsigned long long index) {
			this->index = index;
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