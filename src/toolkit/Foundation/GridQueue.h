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
        void pushQueue(int xx, int yy, int zz);
        int popQueue(int& xx, int& yy, int& zz);


    private:
        gridQueueEle* head ;
        gridQueueEle* tail ;

        queue<gridQueueEle> q;
    };


    GridQueue::GridQueue( ) {
        head = NULL ;
        tail = NULL ;
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

        q.push(*ele);
    }

    int GridQueue::popQueue( int& xx, int& yy, int& zz ) {
        if ( head == NULL )
        {
            return 0 ;
        }

        xx = head->x ;
        yy = head->y ;
        zz = head->z ;

        gridQueueEle res = q.front();
        q.pop();

//        xx = res.x;
//        yy = res.y;
//        zz = res.z;

        gridQueueEle* temp = head ;
        head = head->next ;
        delete temp ;

        if ( head == NULL )
        {
            tail = NULL ;
        }

        return 1 ;
    }

}

#endif
