
#include "stdplus.h"


struct QUEUE *QueueCreate(const size_t size)  
{  
	
	struct QUEUE *q = NULL;
	
	SP_ASSERT(size);
	q = (struct QUEUE *)malloc(sizeof(struct QUEUE));
	if(!q)
		return NULL;

	q->pbuf = (size_t *)malloc(sizeof(size_t) * size);
	if(!q->pbuf)
		return NULL;

	memset(q->pbuf, 0, size);
    q->indentification.front = 0;      
    q->indentification.rear = 0;
	q->indentification.size = size;
	
    return q;  
}  

void QueueClear(struct QUEUE *q)
{
	 q->indentification.front = q->indentification.rear;
	 q->indentification.size = 0;
}

size_t QueueGetItemCount(struct QUEUE *q)
{
	if(q->indentification.rear > q->indentification.front){
		return q->indentification.rear - q->indentification.front;
	}else{
		return q->indentification.rear + q->indentification.size - q->indentification.front;
	}
	
}

bool QueuePush(struct QUEUE *q, const size_t it)  
{  
	struct QUEUE_INFO_INDENTIFICATION *qi = NULL;

	SP_ASSERT(q);

    if(QueueIsFull(q))  
        return false;  

	q->pbuf[q->indentification.rear] = it;
	
	qi = &q->indentification;
	qi->rear = (qi->rear + 1)%qi->size;

    return true;
}  

bool QueuePop(struct QUEUE *q, size_t *item)  
{  
	struct QUEUE_INFO_INDENTIFICATION *qi = NULL;


	SP_ASSERT(q);

    if(QueueIsEmpty(q)) 
		return false;

	qi = &q->indentification;
	*item = q->pbuf[qi->front];
	qi->front = (qi->front+1)%qi->size;

	return true;
}  

bool QueuePopBack(struct QUEUE *q, size_t *item)  
{  
	struct QUEUE_INFO_INDENTIFICATION *qi = NULL;


	SP_ASSERT(q);

    if(QueueIsEmpty(q)) 
		return false;

	qi = &q->indentification;
	*item = q->pbuf[qi->rear];
	q->pbuf[qi->rear] = 0;
	qi->rear = (qi->rear)? (qi->rear-1) : qi->size;

	return true;
}  




bool QueueIsFull(const struct QUEUE *q)  
{  
	return ((q->indentification.rear+1) % q->indentification.size ==\
			q->indentification.front ) ? true : false; 
} 

bool QueueIsEmpty(const struct QUEUE *q)  
{  
	return q->indentification.rear == q->indentification.front  ? true : false; 
}





