#ifndef MEMORYLEAKDETECTION_H
#define MEMORYLEAKDETECTION_H

#ifdef _DEBUG
#include <string.h>
#include <list>
#include <iostream>
using namespace std;

typedef unsigned long DWORD;
typedef struct {
	DWORD	address;
	DWORD	size;
	char	fileName[64];
	DWORD	lineNumber;
} ALLOCATED_INFO;

typedef list<ALLOCATED_INFO*> AllocatedList;

AllocatedList *allocatedList;

void addTrace(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum) {
	ALLOCATED_INFO *info;
	if(!allocatedList) {
		allocatedList = new(AllocatedList);
	}

	info = new(ALLOCATED_INFO);
	info->address = addr;
	strncpy(info->fileName, fname, 63);
	info->lineNumber = lnum;
	info->size = asize;
	allocatedList->insert(allocatedList->begin(), info);
};

void removeTrace(DWORD addr) {
	AllocatedList::iterator i;
	if(!allocatedList)
		return;
	for(i = allocatedList->begin(); i != allocatedList->end(); i++) {
		if((*i)->address == addr) {
			allocatedList->remove((*i));
			break;
		}
	}
};

#define new new(__FILE__, __LINE__)
inline void * __cdecl operator new(size_t size, const char *fileName, int lineNumber) {
	void *ptr = (void *)malloc(size);
	addTrace((DWORD)ptr, size, fileName, lineNumber);
	return(ptr);
};

void __cdecl operator delete(void *p) {
	removeTrace((DWORD)p);
	free(p);
};
      

void Dump() {
	AllocatedList::iterator i;
	DWORD totalSize = 0;

	if(!allocatedList)
		return;

	for(i = allocatedList->begin(); i != allocatedList->end(); i++) {
		printf("%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d NOTFREED\n", (*i)->fileName, (*i)->lineNumber, (*i)->address, (*i)->size);
		totalSize += (*i)->size;
	}
	printf("\n-----------------------------------------------------------\n");
	if(!totalSize) {
		printf("There are no MEMORY LEAKS\n");
	} else {
		printf("Total UNFREED MEMORY: %d bytes\n", totalSize);
	}
};
#else
#define new new
#endif

#endif