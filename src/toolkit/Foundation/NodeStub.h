/*
 * NodeStub.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_FOUNDATION_NODESTUB_H_
#define SRC_TOOLKIT_FOUNDATION_NODESTUB_H_

#include <iostream>

using std::ostream;

namespace GraphMatch {

    class NodeStub {
        public:
            char n1Node;
            char n2Node;
            NodeStub * parentNode;
        public:
            NodeStub();
            ~NodeStub();
    };

    inline ostream& operator<<(ostream &out, const NodeStub& obj) {
        return out<<(int)obj.n1Node
                  <<" "<<(int)obj.n2Node
                  <<" "<<obj.parentNode
                  <<std::endl;
    } 

} /* namespace GraphMatch */

#endif /* SRC_TOOLKIT_FOUNDATION_NODESTUB_H_ */
