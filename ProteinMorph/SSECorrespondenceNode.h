// Copyright (C) 2005-2009 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A correspondence node

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.3  2010/04/27 21:10:17  ssa1
//   Implementing Cost-matrix based SSE Registration and performance optimizations on graph construction
//
//   Revision 1.2  2009/11/19 18:19:25  ssa1
//   Improved flexible fitting.. (Split nodes to guarantee direction)
//
//   Revision 1.1  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//


#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_NODE_H
#define PROTEINMORPH_SSE_CORRESPONDENCE_NODE_H

#include <MathTools/Vector3D.h>
#include <cassert>

using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Protein_Morph {
		class SSECorrespondenceNode {
		public:
			SSECorrespondenceNode();
			SSECorrespondenceNode(unsigned int pIndex, unsigned int qIndex, bool isForward);
			unsigned int GetPIndex();
			unsigned int GetQIndex();
			bool IsForward(); // Specifies whether q is aligned in the same direction as p, or whether q has swapped directions.
			void SetForward(bool in);
		private:
			unsigned int pIndex;
			unsigned int qIndex;
			bool directionForward;
		};

		SSECorrespondenceNode::SSECorrespondenceNode() {
			this->pIndex = 0;
			this->qIndex = 0;
			directionForward = true;
		}

		SSECorrespondenceNode::SSECorrespondenceNode(unsigned int pIndex, unsigned int qIndex, bool isForward) {
			this->pIndex = pIndex;
			this->qIndex = qIndex;
			directionForward = isForward;
		}

		unsigned int SSECorrespondenceNode::GetPIndex() {
			return pIndex;
		}

		unsigned int SSECorrespondenceNode::GetQIndex(){ 
			return qIndex;
		}

		bool SSECorrespondenceNode::IsForward() {
			return directionForward;
		}

		void SSECorrespondenceNode::SetForward(bool in){
			directionForward = in;
		}

	}
}

#endif