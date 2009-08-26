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


#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_NODE_H
#define PROTEINMORPH_SSE_CORRESPONDENCE_NODE_H

#include <MathTools/Vector3D.h>
#include <cassert>

using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Protein_Morph {
		class SSECorrespondenceNode {
		public:
			SSECorrespondenceNode(unsigned int pIndex, unsigned int qIndex);
			unsigned int GetPIndex();
			unsigned int GetQIndex();
		private:
			unsigned int pIndex;
			unsigned int qIndex;
		};

		SSECorrespondenceNode::SSECorrespondenceNode(unsigned int pIndex, unsigned int qIndex) {
			this->pIndex = pIndex;
			this->qIndex = qIndex;
		}

		unsigned int SSECorrespondenceNode::GetPIndex() {
			return pIndex;
		}

		unsigned int SSECorrespondenceNode::GetQIndex(){ 
			return qIndex;
		}

	}
}

#endif