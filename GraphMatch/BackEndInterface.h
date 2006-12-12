#ifndef BACK_END_INTERFACE_H
#define BACK_END_INTERFACE_H

#include "GlobalConstants.h"
#include "QueryEngine.h"
#include "LinkedNode.h"

using namespace std;

public class BackEndInterface {
public:
	BackEndInterface();
	~BackEndInterface();
	// Initialization Methods
	void SetConstantsFromFile(char * fileName);
	bool SetConstant(char * token, char * value);
	bool SetConstant(char * token, double value);
	bool SetConstant(char * token, int value);
	bool SetConstant(char * token, bool value);
	// Process Execution
	int ExecuteQuery();
	// Result Retrieval
	LinkedNode * GetResult(int rank);
	// Cleanup
	void CleanupMemory();
private:
	QueryEngine * queryEngine;
};

BackEndInterface::BackEndInterface() {
	queryEngine = new QueryEngine();
}

BackEndInterface::~BackEndInterface() {
	delete queryEngine;
}

void BackEndInterface::SetConstantsFromFile(char * fileName) {
	LoadConstantsFromFile(fileName);	
}

bool BackEndInterface::SetConstant(char * token, char * value) {
	return SetConstantFromToken(token, value, 0.0, 0, false);
}

bool BackEndInterface::SetConstant(char *token, double value) {
	return SetConstantFromToken(token, NULL, value, 0, false);
}

bool BackEndInterface::SetConstant(char *token, int value) {
	return SetConstantFromToken(token, NULL, 0.0, value, false);
}

bool BackEndInterface::SetConstant(char *token, bool value) {
	return SetConstantFromToken(token, NULL, 0.0, 0, value);
}

int BackEndInterface::ExecuteQuery() {
	return queryEngine->DoGraphMatching();
}

LinkedNode * BackEndInterface::GetResult(int rank) {
	return queryEngine->GetSolution(rank);
}

void BackEndInterface::CleanupMemory() {
	queryEngine->FinishGraphMatching();
}

#endif