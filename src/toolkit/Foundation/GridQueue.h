#ifndef TOOLKIT_SKELETON_MAKER_GRID_QUEUE_H
#define TOOLKIT_SKELETON_MAKER_GRID_QUEUE_H

#include <cstdlib>

using namespace std;

namespace SkeletonMaker {
    struct gridQueueEle
    {
        int x, y, z;
        int score ;
        gridQueueEle* next ;
    };

    class GridQueue
    {
    public:
        GridQueue();
        gridQueueEle* getHead();
        int getNumElements();
        void pushQueue(int xx, int yy, int zz);
        int popQueue(int& xx, int& yy, int& zz);


    private:
        void swapEle(gridQueueEle* pre, gridQueueEle* e1, gridQueueEle* e2);
    private:
        gridQueueEle* head ;
        gridQueueEle* tail ;
        int numEles ;
    };


    GridQueue::GridQueue( ) {
        head = NULL ;
        tail = NULL ;
        numEles = 0 ;
    }

    gridQueueEle* GridQueue::getHead( ) {
        return head ;
    }

    int GridQueue::getNumElements( ) {
        return numEles ;
    }

    void GridQueue::pushQueue( int xx, int yy, int zz ) {
        gridQueueEle* ele = new gridQueueEle ;
        ele->x = xx ;
        ele->y = yy ;
        ele->z = zz ;
        ele->score = 0 ;
        ele->next = NULL ;
        if ( head == NULL )
        {
            head = ele ;
        }
        else
        {
            tail->next = ele ;
        }
        tail = ele ;
        numEles ++ ;
    }

    int GridQueue::popQueue( int& xx, int& yy, int& zz ) {
        if ( head == NULL )
        {
            return 0 ;
        }

        xx = head->x ;
        yy = head->y ;
        zz = head->z ;

        gridQueueEle* temp = head ;
        head = head->next ;
        delete temp ;

        if ( head == NULL )
        {
            tail = NULL ;
        }
        numEles -- ;

        return 1 ;
    }

    /* Switching two elements */
    void GridQueue::swapEle( gridQueueEle* pre, gridQueueEle* e1, gridQueueEle* e2 )
    {
        if ( pre != NULL )
        {
            e1->next = e2->next ;
            e2->next = pre->next ;
            pre->next = e2 ;

            if ( tail == e2 )
            {
                tail = e1 ;
            }
        }
        else
        {
            e1->next = e2->next ;
            e2->next = e1 ;
            head = e2 ;

            if ( tail == e2 )
            {
                tail = e1 ;
            }
        }
    }


}

#endif
