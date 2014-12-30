/* Includes */
#include <stdio.h>      
#include <pthread.h>    
#include <string.h>
#include <urcu-bp.h>  
#include <unistd.h>

void print_message_function ( void *ptr );
void update_message_function ( void *ptr );
void rcu_callback_func(struct rcu_head *head);

typedef struct str_thdata
{
	int thread_no;
	char message[40];
	struct rcu_head rcu;    // this structure is required for call_rcu // user level call_rcu littlebit defferent compared to kernel level; 

} THREAD_DATA_T;


THREAD_DATA_T *gdata = NULL; 

int main()
{
	pthread_t thread1, thread2,thread3;  

	gdata =  (THREAD_DATA_T*) malloc(sizeof(THREAD_DATA_T)); 

	/* initialize data to pass to thread 1 */
	gdata->thread_no = 110;

	strcpy(gdata->message, "Hello!");


	/* create threads 1 and 2 */    
	pthread_create (&thread1, NULL, (void *) print_message_function, NULL);
	pthread_create (&thread2, NULL, (void *) print_message_function, NULL);
	pthread_create (&thread3, NULL, (void *) update_message_function,NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	exit(0);
}


void print_message_function ( void *ptr )
{

	THREAD_DATA_T *deref_data = NULL; 

	pthread_t  tid = pthread_self();


	while(1)
	{   
		rcu_read_lock();	   
		deref_data = rcu_dereference(gdata);
		printf("\n%u Thread---->  data = %d says %s Anand ---->> I got read lock  \n",(unsigned int)tid, deref_data->thread_no, deref_data->message);
		sleep(17);
		rcu_read_unlock();

	}

	pthread_exit(0); 
}



void update_message_function ( void *ptr )
{

	THREAD_DATA_T *data = NULL; 
	THREAD_DATA_T *copydata =NULL; 
	clock_t t;

	
	pthread_t  tid = pthread_self();
	while(1)
	{   
		data =  gdata;
		copydata =(THREAD_DATA_T*)malloc(sizeof(THREAD_DATA_T) );
		*copydata = *data;

		copydata->thread_no++;
		printf("\naddr =%X Thread  %u -->  I am  assigning to actual pointer ..data %d, copy data=%d\n ",data, (unsigned int)tid ,gdata->thread_no,copydata->thread_no);

		rcu_assign_pointer(gdata,copydata);
		
		//synchronize_rcu();  
		call_rcu(&data->rcu,rcu_callback_func);   // non blocking call ....  rcu_callback_func will call after grace period .
		printf("\ngrace period =%f sec\n",((double)t)/CLOCKS_PER_SEC);
		//free(data);   //dont free the memory ..may be this memory is refering in read side .. wait all reader to complete and do free after geace period .
		sleep(5);   /// just introduced delay to see output

		printf("\nThread %u  -->I am  assigning to actual pointer done ....data= %d,copy data=%d\n ",(unsigned int)tid ,gdata->thread_no,copydata->thread_no);

	}
	pthread_exit(0); 
}


// This is call back function , It is called after grace period  
void rcu_callback_func(struct rcu_head *head)
{
	THREAD_DATA_T  *p = caa_container_of(head,THREAD_DATA_T,rcu);   //caa_container_of - Get the address of an object containing a field
	printf("\nHi Anand this rcu call back ...%X\n",p);
	free(p);
}
