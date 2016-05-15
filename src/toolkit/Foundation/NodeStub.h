/*
 * NodeStub.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_FOUNDATION_NODESTUB_H_
#define SRC_TOOLKIT_FOUNDATION_NODESTUB_H_

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

} /* namespace GraphMatch */

#endif /* SRC_TOOLKIT_FOUNDATION_NODESTUB_H_ */
