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
        // Initialization Methods
        void load(string fileName);
        bool setConstant(string token, string value);
        bool setConstant(string token, double value);
        bool setConstant(string token, int value);
        bool setConstant(string token, bool value);
        string getConstantString(string token);
        double getConstantDouble(string token);
        int getConstantInt(string token);
        bool getConstantBool(string token);
        void clearAllConstraints();

        // Process Execution
        virtual int run();
        // Cleanup
        void cleanupMemory();
    protected:
    };

    inline void IBackEnd::load(string fileName) {
        LoadConstantsFromFile(fileName);
    }

    inline bool IBackEnd::setConstant(string token, string value) {
        return setConstantFree(token, value);
    }

    inline bool IBackEnd::setConstant(string token, double value) {
        return setConstantFree(token, value);
    }

    inline bool IBackEnd::setConstant(string token, int value) {
        return setConstantFree(token, value);
    }

    inline bool IBackEnd::setConstant(string token, bool value) {
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

    inline int IBackEnd::run() {
        if(skeleton != NULL && sequence != NULL)
            return Matcher::run(*sequence, *skeleton);
        else
            return 0;
    }

    inline void IBackEnd::cleanupMemory() {
        destruct();
    }
}

#endif
