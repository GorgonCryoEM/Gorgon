// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing an base graph vertex

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/Foundation/GraphVertexBase.h,v $
//   $Revision: 1.6 $
//   $Date: 2010/05/26 15:13:05 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: GraphVertexBase.h,v $
//   Revision 1.6  2010/05/26 15:13:05  ssa1
//   fixing linux compilation error
//
//   Revision 1.5  2010/04/27 17:30:54  ssa1
//   SSE Registration search by first finding all cliques, and then finding the matching.
//
//   Revision 1.4  2009/11/04 20:29:38  ssa1
//   Implementing Triangle based clique search and chain based flexible fitting.
//
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