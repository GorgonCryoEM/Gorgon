#ifndef CORE_SKELETON_MAKER_GRID_QUEUE2_H
#define CORE_SKELETON_MAKER_GRID_QUEUE2_H

#include "GridQueue.h"

namespace SkeletonMaker {

    class GridQueue2
    {
    public:
        GridQueue2();
        ~GridQueue2();
        gridQueueEle* getNext();
        void reset();
        int getNumElements();
        void prepend(int xx, int yy, int zz);
        gridQueueEle * remove();
        gridQueueEle* swap();
    private:
        gridQueueEle* head ;
        gridQueueEle* pre ;
        gridQueueEle* prepre ;
        gridQueueEle* cur ;
        int numEles ;
    };

    GridQueue2::GridQueue2( )
    {
        head = NULL ;
        cur = NULL ;
        pre = NULL ;
        prepre = NULL ;
        numEles = 0 ;
    }

    GridQueue2::~GridQueue2()
    {
        gridQueueEle* ele;
        reset();
        ele=getNext();
        while ( (ele=remove()) != NULL ){};
    }
    gridQueueEle* GridQueue2::getNext( )
    {
        if ( cur == NULL )
        {
            prepre = NULL ;
            pre = NULL ;
            cur = head ;
        }
        else
        {
            prepre = pre ;
            pre = cur ;
            cur = cur->next ;
        }

        return cur ;
    }

    void GridQueue2::reset( )
    {
        prepre = NULL ;
        pre = NULL ;
        cur = NULL ;
    }

    int GridQueue2::getNumElements( )
    {
        return numEles ;
    }

    void GridQueue2::prepend( int xx, int yy, int zz )
    {
        gridQueueEle* ele = new gridQueueEle ;
        ele->x = xx ;
        ele->y = yy ;
        ele->z = zz ;
        ele->score = 0 ;
        ele->next = head;
        head = ele ;
        numEles ++ ;

        reset() ;
    }

    /* Remove current element pointed by cur */
    gridQueueEle * GridQueue2::remove( )
    {
        gridQueueEle* temp = cur ;
        if ( cur != NULL )
        {
            cur = cur->next ;
            delete temp ;

            if ( pre != NULL )
            {
                pre->next = cur ;
            }
            else
            {
                head = cur ;
            }
            numEles -- ;
        }


        return cur ;
    }

    /* Switching pre and cur */
    gridQueueEle * GridQueue2::swap( )
    {
        if ( prepre != NULL )
        {
            pre->next = cur->next ;
            cur->next = prepre->next ;
            prepre->next = cur ;

        }
        else
        {
            pre->next = cur->next ;
            cur->next = pre ;
            head = cur ;
        }

        gridQueueEle* temp = pre ;
        pre = cur ;
        cur = temp ;

        return cur ;
    }


}

#endif
