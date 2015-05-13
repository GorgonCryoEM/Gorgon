// Copyright (C) 2005-2009 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A correspondence node



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
