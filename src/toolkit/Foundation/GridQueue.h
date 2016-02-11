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
        queue<gridQueueEle> q;
    };


    GridQueue::GridQueue( ) {
    }

    void GridQueue::pushQueue( int xx, int yy, int zz ) {
        gridQueueEle ele;
        ele.x = xx ;
        ele.y = yy ;
        ele.z = zz ;
        ele.score = 0 ;
        ele.next = NULL ;

        q.push(ele);
    }

    int GridQueue::popQueue( int& xx, int& yy, int& zz ) {
        if ( q.size() == 0 )
        {
            return 0 ;
        }

        gridQueueEle res = q.front();
        q.pop();

        xx = res.x;
        yy = res.y;
        zz = res.z;

        return 1 ;
    }

}

#endif
