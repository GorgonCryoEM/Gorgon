#include "PriorityQueue.h"

void main ( int args, char* argv[] )
{
	PriorityQueue <int,int> * myq = new PriorityQueue <int,int> ( 100 );

	for ( int i = 0 ; i < 90 ; i ++ )
	{
		int* ipt = new int ;
		*ipt = i ;
		myq->add(ipt, ( i - 30 ) * ( i - 30 ) );
	}

	int j; 
	while ( j < 1000 )
	{
		int* ipt;
		
		myq->remove( ipt, j ) ;
		printf("Key: %d Content: %d\n", j, *ipt ) ;
	}
	
	delete myq ;
}