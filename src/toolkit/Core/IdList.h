/*
 * IdList.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_CORE_IDLIST_H_
#define SRC_TOOLKIT_CORE_IDLIST_H_

#include "Output.h"

namespace Core {

    typedef unsigned long long TKey;
    typedef vector<TKey> CElem;

    class IdList {
        public:
            IdList();
            virtual ~IdList();

            CElem getIds() const;
            void addId(TKey i);
        private:
            CElem ids;

            friend ostream& operator<<(ostream& out, const IdList& obj);
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_CORE_IDLIST_H_ */
