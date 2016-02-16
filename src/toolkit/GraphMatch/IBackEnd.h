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
        StandardGraph * skeleton;
        StandardGraph * sequence;
    };

    IBackEnd::IBackEnd(): skeleton(NULL), sequence(NULL) {
        matcher = new Matcher();
    }

    IBackEnd::~IBackEnd() {
        delete matcher;
        if(skeleton != NULL) {
            delete skeleton;
        }
        if(sequence != NULL) {
            delete sequence;
        }
    }

    void IBackEnd::load(string fileName) {
        LoadConstantsFromFile(fileName);
    }

    bool IBackEnd::SetConstant(string token, string value) {
        return setConstantFree(token, value);
    }

    bool IBackEnd::SetConstant(string token, double value) {
        return setConstantFree(token, value);
    }

    bool IBackEnd::SetConstant(string token, int value) {
        return setConstantFree(token, value);
    }

    bool IBackEnd::SetConstant(string token, bool value) {
        return setConstantFree(token, value);
    }

    string IBackEnd::GetConstantString(string token) {
        string  sVal;
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, sVal, dVal, iVal, bVal);
        return sVal;
    }

    double IBackEnd::GetConstantDouble(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return dVal;
    }

    int IBackEnd::GetConstantInt(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return iVal;
    }

    bool IBackEnd::GetConstantBool(string token) {
        int iVal;
        double dVal;
        bool bVal;
        GetConstantFromToken(token, "", dVal, iVal, bVal);
        return bVal;
    }

    void IBackEnd::ClearAllConstraints() {
        ClearAllowedConstraints();
        ClearNotAllowedConstraints();
    }

    void IBackEnd::loadSequence() {
        #ifdef GORGON_DEBUG
        cout << "In BackEndInterface::LoadSequenceGraph" << endl;
        #endif
        if(sequence != NULL) {
            delete sequence;
        }
        sequence = matcher->loadSequence();
        //sequence->PrintGraph();
    }

    void IBackEnd::loadSkeleton() {
        if(skeleton != NULL) {
            delete skeleton;
        }
        skeleton = matcher->loadSkeleton();
        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   BackEndInterface.h"<<endl;
              cout<<"DEBUG: Method: BackEndInterface::LoadSkeletonGraph()\033[0m"<<endl;
              skeleton->PrintGraph();
              cout<<"\033[32mDEBUG: END: File:   BackEndInterface.h\033[0m"<<endl;
        #endif

    }

    int IBackEnd::ExecuteQuery() {
        if(skeleton != NULL && sequence != NULL)
            return matcher->match(sequence, skeleton);
        else
            return 0;
    }

    SSEResult IBackEnd::GetResult(int rank) {
        return matcher->GetSolution(rank);
    }

    void IBackEnd::CleanupMemory() {
        matcher->destruct();
    }
}

#endif
