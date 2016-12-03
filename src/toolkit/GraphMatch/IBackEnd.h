#ifndef TOOLKIT_GRAPHMATCH_BACK_END_INTERFACE_H
#define TOOLKIT_GRAPHMATCH_BACK_END_INTERFACE_H

#include "Constants.h"

using namespace std;

namespace GraphMatch {

    class IBackEnd {
    public:
        // Initialization Methods
        void loadConstantsFromFile(string fileName);
        bool setConstantString(string token, string value);
        bool setConstantDouble(string token, double value);
        bool setConstantInt(string token, int value);
        bool setConstantBool(string token, bool value);
        string getConstantString(string token);
        double getConstantDouble(string token);
        int getConstantInt(string token);
        bool getConstantBool(string token);
        void clearAllConstraints();
    };

    inline void IBackEnd::loadConstantsFromFile(string fileName) {
        LoadConstantsFromFile(fileName);
    }

    inline bool IBackEnd::setConstantString(string token, string value) {
#ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   IBackEnd.h"<<endl;
        cout<<"inline bool IBackEnd::setConstantString()\n";
        cout<<" token: "<<token
            <<" value: "<<value
            <<endl;
#endif

        return setConstant(token, value);
    }

    inline bool IBackEnd::setConstantDouble(string token, double value) {
#ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   IBackEnd.h"<<endl;
        cout<<"inline bool IBackEnd::setConstantDouble()\n";
        cout<<" token: "<<token
            <<" value: "<<value
            <<endl;
#endif

        return setConstant(token, value);
    }

    inline bool IBackEnd::setConstantInt(string token, int value) {
#ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   IBackEnd.h"<<endl;
        cout<<"inline bool IBackEnd::setConstantInt()\n";
        cout<<" token: "<<token
            <<" value: "<<value
            <<endl;
#endif

        return setConstant(token, value);
    }

    inline bool IBackEnd::setConstantBool(string token, bool value) {
#ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   IBackEnd.h"<<endl;
        cout<<"inline bool IBackEnd::setConstantBool()\n";
        cout<<" token: "<<token
            <<" value: "<<value
            <<endl;
#endif

        return setConstant(token, value);
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
}

#endif
