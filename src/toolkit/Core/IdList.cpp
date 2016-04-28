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

    IdList::IdList(TKey i1, TKey i2)
            : ids(2)
    {
        ids[0] = i1;
        ids[1] = i2;
    }

    TKey IdList::id(int i) const {
        return ids[i];
    }

    unsigned int IdList::sizeIds() const {
        return ids.size();
    }

    ostream& operator<<(ostream& out, const IdList& obj) {
        return out //<<"\033[34m"
                    << "\n\nvertexIds.size(): " << obj.ids.size()
                    << endl
                    << obj.ids << endl << endl;
    }

} /* namespace Core */
