#ifndef TOOLKIT_GRAPHMATCH_SECONDARYSTRUCTURE_H
#define TOOLKIT_GRAPHMATCH_SECONDARYSTRUCTURE_H

#include <string>
#include "GraphMatch/Constants.h"

using namespace std;

namespace GraphMatch {

    class SecStruct {
    public:
        int sseType;
        int serialNumber;
        int startPosition;
        int endPosition;
        string ID;

    public:
        bool IsHelix();
        bool IsSheet();
        int GetLengthResidues();
        int GetLengthBonds();
        float GetLengthAngstroms();
        int GetSerialNumber();
        int GetStartPosition();
        int GetEndPosition();
        string getID();
    };

    inline bool SecStruct::IsHelix() {
        return (sseType == GRAPHEDGE_HELIX);
    }

    inline bool SecStruct::IsSheet() {
        return (sseType == GRAPHEDGE_SHEET);
    }

    inline int SecStruct::GetLengthResidues() {
        return (endPosition - startPosition + 1);
    }

    inline int SecStruct::GetLengthBonds() {
        return (endPosition - startPosition);
    }

    inline float SecStruct::GetLengthAngstroms() {
        if(IsHelix()) {
            return (float)(endPosition - startPosition + 1) * HELIX_C_ALPHA_TO_ANGSTROMS;
        } else {
            return (float)(endPosition - startPosition + 1) * LOOP_C_ALPHA_TO_ANGSTROMS;
        }
    }

    inline int SecStruct::GetSerialNumber() {
        return serialNumber;
    }

    inline int SecStruct::GetStartPosition() {
        return startPosition;
    }

    inline int SecStruct::GetEndPosition() {
        return endPosition;
    }

    inline string SecStruct::getID() {
        return ID;
    }
}

#endif
