// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for interactively creating and modifying c-alpha loops

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$


#ifndef GORGON_INTERACTIVE_LOOP_BUILDER_ENGINE_H
#define GORGON_INTERACTIVE_LOOP_BUILDER_ENGINE_H

#include "InteractiveSkeletonEngine.h"

namespace wustl_mm {
	namespace Visualization {	
		typedef map<unsigned long long, bool> SketchMapType;
		class InteractiveLoopBuilderEngine : public InteractiveSkeletonEngine {
		public:
			InteractiveLoopBuilderEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float minGray, int stepCount, int curveRadius, int minCurveSize, unsigned int medialnessScoringFunction);
		};

		InteractiveLoopBuilderEngine::InteractiveLoopBuilderEngine(Volume * volume, NonManifoldMesh_Annotated * skeleton, float minGray, int stepCount, int curveRadius, int minCurveSize, unsigned int medialnessScoringFunction) 
			: InteractiveSkeletonEngine(volume, skeleton, minGray, stepCount, curveRadius, minCurveSize, medialnessScoringFunction){
		}
	}
}

#endif