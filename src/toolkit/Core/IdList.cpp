/*
 * IdList.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "IdList.h"

namespace Core {

    IdList::IdList() {}

    IdList::~IdList() {}

    CElem IdList::getIds() const {
        return ids;
    }

    void IdList::addId(TKey i) {
        ids.push_back(i);
    }

    ostream& operator<<(ostream& out, const IdList& obj) {
        return out //<<"\033[34m"
                    << "\n\nvertexIds.size(): " << obj.ids.size()
                    << endl
                    << obj.ids << endl << endl;
    }

} /* namespace Core */
