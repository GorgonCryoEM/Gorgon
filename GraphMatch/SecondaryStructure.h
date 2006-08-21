#ifndef SECONDARYSTRUCTURE_H
#define SECONDARYSTRUCTURE_H

class SecondaryStructure {
public:
	int secondaryStructureType;
	int serialNumber;
	int startPosition;
	int endPosition;
	char * secondaryStructureID;

public:
	SecondaryStructure();
	~SecondaryStructure();
	int GetLength();
};
SecondaryStructure::SecondaryStructure() {
	secondaryStructureID = NULL;
}

SecondaryStructure::~SecondaryStructure() {
	if(secondaryStructureID != NULL) {
		delete secondaryStructureID;
	}
}

int SecondaryStructure::GetLength() {
	return endPosition - startPosition + 1;
}


#endif