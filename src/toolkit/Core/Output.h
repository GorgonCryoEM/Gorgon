/*
 * Output.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_CORE_OUTPUT_H_
#define SRC_TOOLKIT_CORE_OUTPUT_H_

#include <iostream>
#include <vector>
#include <map>
#include <set>

using std::ostream;
using std::endl;
using std::vector;
using std::map;
using std::set;

namespace Core {
    template <class T>
    ostream & operator<<(ostream & out, const vector<T> & obj) {
            for(typename vector<T>::const_iterator it=obj.begin();
                    it!=obj.end();
                    ++it)

                out<<"\t\t\t"<<*it
                <<endl;

            return out;
    }

    template <class T, class U>
    inline ostream & operator<<(ostream & out, const map<T, U> & obj) {
            for(typename map<T, U>::const_iterator it = obj.begin();
                    it != obj.end(); ++it)

                out<<"\t"<<it->first
                <<"\t"<<it->second
                <<endl;

            return out;
    }

    template <class T>
    inline ostream & operator<<(ostream & out, const set<T> & obj) {
            for(typename set<T>::const_iterator it = obj.begin(); it != obj.end();
                    ++it)

                out<<"\t"<<*it
                <<endl;

            return out;
    }

}

#endif /* SRC_TOOLKIT_CORE_OUTPUT_H_ */
