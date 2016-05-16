#ifndef TOOLKIT_GRAPHMATCH_BACK_END_INTERFACE_H
#define TOOLKIT_GRAPHMATCH_BACK_END_INTERFACE_H

//#include "Core/GlobalConstants.h"
#include "Matcher.h"
//#include "LinkedNode.h"

#ifdef DEBUG
	#include <iostream>
#endif

using namespace std;

namespace GraphMatch {

    class IBackEnd : public Matcher {
    public:
        IBackEnd(Graph & sequenceGraph, Graph & skeletonGraph);
        // Initialization Methods
        void loadConstantsFromFile(string fileName);
        template <class T>
        bool setConstant(string token, T value);
        string getConstantString(string token);
        double getConstantDouble(string token);
        int getConstantInt(string token);
        bool getConstantBool(string token);
        void clearAllConstraints();

        // Cleanup
        void cleanupMemory();
    protected:
    };

    inline IBackEnd::IBackEnd(Graph & sequenceGraph, Graph & skeletonGraph)
                    : Matcher(sequenceGraph, skeletonGraph)
    {}

    inline void IBackEnd::loadConstantsFromFile(string fileName) {
        LoadConstantsFromFile(fileName);
    }

    template <class T>
    inline bool IBackEnd::setConstant(string token, T value) {
        return setConstantFree(token, value);
    }

    inline string IBackEnd::getConstantString(string token) {
        string  sVal;
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, sVal, dVal, iVal, bVal);
        return sVal;
    }

    inline double IBackEnd::getConstantDouble(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return dVal;
    }

    inline int IBackEnd::getConstantInt(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return iVal;
    }

    inline bool IBackEnd::getConstantBool(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return bVal;
    }

    inline void IBackEnd::clearAllConstraints() {
        ClearAllowedConstraints();
        ClearNotAllowedConstraints();
    }

    inline void IBackEnd::cleanupMemory() {
        destruct();
    }
}

#endif
