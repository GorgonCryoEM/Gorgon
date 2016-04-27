/*
 * Vertex.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_VERTEX_H_
#define SRC_TOOLKIT_VISUALIZATION_VERTEX_H_

#include "MathTools/Vector3.h"

using namespace GraphMatch;

namespace Core {

    typedef unsigned long long TKey;

    template <class T>
    ostream & operator<<(ostream & out, const vector<T> & obj) {
            for(typename vector<T>::const_iterator it=obj.begin();
                    it!=obj.end();
                    ++it)

                out<<"\t\t\t"<<*it
                <<endl;

            return out;
    }

    class Vertex : public Vec3F {
        public:
            Vertex();
            Vertex(Vec3F pos);

            TKey addId(TKey id);
            unsigned int sizeIds() const;
            vector<TKey> getIds() const;
            TKey edge(int id) const;

            TKey sizeEdge() const;

        private:
            vector<TKey> ids;

            friend ostream& operator<<(ostream& out, const Vertex& obj);
    };


}


#endif /* SRC_TOOLKIT_VISUALIZATION_VERTEX_H_ */
