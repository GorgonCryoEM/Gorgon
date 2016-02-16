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

    bool SecStruct::IsHelix() {
        return (sseType == GRAPHEDGE_HELIX);
    }

    bool SecStruct::IsSheet() {
        return (sseType == GRAPHEDGE_SHEET);
    }

    int SecStruct::GetLengthResidues() {
        return (endPosition - startPosition + 1);
    }

    int SecStruct::GetLengthBonds() {
        return (endPosition - startPosition);
    }

    float SecStruct::GetLengthAngstroms() {
        if(IsHelix()) {
            return (float)(endPosition - startPosition + 1) * HELIX_C_ALPHA_TO_ANGSTROMS;
        } else {
            return (float)(endPosition - startPosition + 1) * LOOP_C_ALPHA_TO_ANGSTROMS;
        }
    }

    int SecStruct::GetSerialNumber() {
        return serialNumber;
    }

    int SecStruct::GetStartPosition() {
        return startPosition;
    }

    int SecStruct::GetEndPosition() {
        return endPosition;
    }

    string SecStruct::getID() {
        return ID;
    }
}

#endif
