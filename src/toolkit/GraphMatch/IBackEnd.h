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

    class IBackEnd {
    public:
        IBackEnd();
        virtual ~IBackEnd();
        // Initialization Methods
        void load(string fileName);
        bool SetConstant(string token, string value);
        bool SetConstant(string token, double value);
        bool SetConstant(string token, int value);
        bool SetConstant(string token, bool value);
        string GetConstantString(string token);
        double GetConstantDouble(string token);
        int GetConstantInt(string token);
        bool GetConstantBool(string token);
        void ClearAllConstraints();

        // Graph Loading
        void loadSequence();
        void loadSkeleton();
        // Process Execution
        virtual int ExecuteQuery();
        // Result Retrieval
        virtual SSEResult GetResult(int rank);
        // Cleanup
        void CleanupMemory();
    protected:
        Matcher * matcher;
        Graph * skeleton;
        Graph * sequence;
    };

    inline IBackEnd::IBackEnd(): skeleton(NULL), sequence(NULL) {
        matcher = new Matcher();
    }

    inline IBackEnd::~IBackEnd() {
        delete matcher;
        if(skeleton != NULL) {
            delete skeleton;
        }
        if(sequence != NULL) {
            delete sequence;
        }
    }

    inline void IBackEnd::load(string fileName) {
        LoadConstantsFromFile(fileName);
    }

    inline bool IBackEnd::SetConstant(string token, string value) {
        return setConstantFree(token, value);
    }

    inline bool IBackEnd::SetConstant(string token, double value) {
        return setConstantFree(token, value);
    }

    inline bool IBackEnd::SetConstant(string token, int value) {
        return setConstantFree(token, value);
    }

    inline bool IBackEnd::SetConstant(string token, bool value) {
        return setConstantFree(token, value);
    }

    inline string IBackEnd::GetConstantString(string token) {
        string  sVal;
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, sVal, dVal, iVal, bVal);
        return sVal;
    }

    inline double IBackEnd::GetConstantDouble(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return dVal;
    }

    inline int IBackEnd::GetConstantInt(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return iVal;
    }

    inline bool IBackEnd::GetConstantBool(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return bVal;
    }

    inline void IBackEnd::ClearAllConstraints() {
        ClearAllowedConstraints();
        ClearNotAllowedConstraints();
    }

    inline void IBackEnd::loadSequence() {
        #ifdef GORGON_DEBUG
        cout << "In BackEndInterface::LoadSequenceGraph" << endl;
        #endif
        if(sequence != NULL) {
            delete sequence;
        }
        sequence = matcher->loadSequence();
        //sequence->print();
    }

    inline void IBackEnd::loadSkeleton() {
        if(skeleton != NULL) {
            delete skeleton;
        }
        skeleton = matcher->loadSkeleton();
        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   BackEndInterface.h"<<endl;
              cout<<"DEBUG: Method: BackEndInterface::LoadSkeletonGraph()\033[0m"<<endl;
              skeleton->print();
              cout<<"\033[32mDEBUG: END: File:   BackEndInterface.h\033[0m"<<endl;
        #endif

    }

    inline int IBackEnd::ExecuteQuery() {
        if(skeleton != NULL && sequence != NULL)
            return matcher->match(sequence, skeleton);
        else
            return 0;
    }

    inline SSEResult IBackEnd::GetResult(int rank) {
        return matcher->GetSolution(rank);
    }

    inline void IBackEnd::CleanupMemory() {
        matcher->destruct();
    }
}

#endif
