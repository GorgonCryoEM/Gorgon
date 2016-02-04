#ifndef CORE_GRAPHMATCH_LINKEDNODESTUB_H
#define CORE_GRAPHMATCH_LINKEDNODESTUB_H

//#include "LinkedNode.h"

using namespace std;

namespace GraphMatch {

    class LinkedNodeStub {
    public:
        char n1Node;
        char n2Node;
        LinkedNodeStub * parentNode;
    public:
        LinkedNodeStub();
        LinkedNodeStub(LinkedNodeStub * oldNode);
        ~LinkedNodeStub();
    };

    LinkedNodeStub::LinkedNodeStub() {
        n1Node = 0;
        n2Node = 0;
        parentNode = NULL;
    }

    LinkedNodeStub::LinkedNodeStub(LinkedNodeStub * oldNode) {
        n1Node = oldNode->n1Node;
        n2Node = oldNode->n2Node;
        parentNode = oldNode->parentNode;
    }

    LinkedNodeStub::~LinkedNodeStub() {
    }
}
#endif
