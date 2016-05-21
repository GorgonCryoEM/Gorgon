#ifndef TOOLKIT_GRAPHMATCH_SECONDARYSTRUCTURE_H
#define TOOLKIT_GRAPHMATCH_SECONDARYSTRUCTURE_H

#include <string>
#include "GraphMatch/Constants.h"

using namespace std;

namespace GraphMatch {

    class SecondaryStructure {
    public:
        int sseType;
        int serialNumber;
        int startPosition;
        int endPosition;
        string secondaryStructureID;

    public:
        bool IsHelix();
        bool IsSheet();
        int GetLengthResidues();
        int GetLengthBonds();
        float GetLengthAngstroms();
        int GetSerialNumber();
        int GetStartPosition();
        int GetEndPosition();
        string GetSecondaryStructureID();
    };

    bool SecondaryStructure::IsHelix() {
        return (sseType == GRAPHEDGE_HELIX);
    }

    bool SecondaryStructure::IsSheet() {
        return (sseType == GRAPHEDGE_SHEET);
    }

    int SecondaryStructure::GetLengthResidues() {
        return (endPosition - startPosition + 1);
    }

    int SecondaryStructure::GetLengthBonds() {
        return (endPosition - startPosition);
    }

    float SecondaryStructure::GetLengthAngstroms() {
        if(IsHelix()) {
            return (float)(endPosition - startPosition + 1) * HELIX_C_ALPHA_TO_ANGSTROMS;
        } else {
            return (float)(endPosition - startPosition + 1) * LOOP_C_ALPHA_TO_ANGSTROMS;
        }
    }

    int SecondaryStructure::GetSerialNumber() {
        return serialNumber;
    }

    int SecondaryStructure::GetStartPosition() {
        return startPosition;
    }

    int SecondaryStructure::GetEndPosition() {
        return endPosition;
    }

    string SecondaryStructure::GetSecondaryStructureID() {
        return secondaryStructureID;
    }
}

#endif
