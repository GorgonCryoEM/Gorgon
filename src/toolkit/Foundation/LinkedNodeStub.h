#ifndef SRC_TOOLKIT_FOUNDATION_LINKEDNODESTUB_H
#define SRC_TOOLKIT_FOUNDATION_LINKEDNODESTUB_H

//#include "LinkedNode.h"
#include <cstdlib>

namespace GraphMatch {

    class LinkedNodeStub {
    public:
        char n1Node;
        char n2Node;
        LinkedNodeStub * parentNode;
    public:
        LinkedNodeStub();
        ~LinkedNodeStub();
    };

    LinkedNodeStub::LinkedNodeStub() {
        n1Node = 0;
        n2Node = 0;
        parentNode = NULL;
    }

    LinkedNodeStub::~LinkedNodeStub() {
    }
}
#endif
