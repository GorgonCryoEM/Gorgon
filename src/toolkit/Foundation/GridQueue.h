#ifndef TOOLKIT_SKELETON_MAKER_GRID_QUEUE_H
#define TOOLKIT_SKELETON_MAKER_GRID_QUEUE_H

#include <cstdlib>
#include <queue>

using namespace std;

namespace SkeletonMaker {
    struct gridQueueEle
    {
        int x, y, z;
        int score ;
        gridQueueEle* next ;

        gridQueueEle(int xx, int yy, int zz)
                : x(xx), y(yy), z(zz)
        {}

        gridQueueEle()
        {}

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
        gridQueueEle* head ;
        gridQueueEle* tail ;
        int numEles ;

        queue<gridQueueEle> q;
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

        q.push(gridQueueEle(xx, yy, zz));
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

}

#endif
