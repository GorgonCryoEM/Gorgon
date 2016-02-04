#ifndef SHEET_H
#define SHEET_H

class SheetStrand {
public:
	int strandNum;
	char * sheetID;
	int numStrands;
	char * initialResidueName;
	char * initialChainID;
	int initialSequenceNumber;
	char * initialICode;
	char * endResidueName;
	char * endChainID;
	int endSequenceNumber;
	char * endICode;
	int sense;
	char * currentAtom;
	char * currentResidueName;
	char * currentChainID;
	int currentResidueSequence;
	char * currentICode;
	char * previousAtomName;
	char * previousResidueName;
	char * previousChainId;
	int previousResidueSequence;
	char * previousICode;
	
};

#endif