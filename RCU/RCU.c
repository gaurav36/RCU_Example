/* Includes */
#include <stdio.h>      
#include <pthread.h>    
#include <string.h>
#include <urcu-bp.h>  

void print_message_function ( void *ptr );
void update_message_function ( void *ptr );

typedef struct str_thdata
{
	int thread_no;
	char message[40];
} THREAD_DATA_T;


THREAD_DATA_T *gdata = NULL; 

int main()
{
	pthread_t thread1, thread2,thread3;  

	gdata =  (THREAD_DATA_T*) malloc(sizeof(THREAD_DATA_T)); 

	/* initialize data to pass to thread 1 */
	gdata->thread_no = 1;

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
		printf("\nThread  %u -->  I am  assigning to actual pointer ..data %d, copy data=%d\n ",(unsigned int)tid ,gdata->thread_no,copydata->thread_no);

		rcu_assign_pointer(gdata,copydata);
		
    		t = clock();
		synchronize_rcu();
		t=clock()- t;
		printf("\ngrace period =%f sec\n",((double)t)/CLOCKS_PER_SEC);
		free(data);

		printf("\nThread %u  -->I am  assigning to actual pointer done ....data= %d,copy data=%d\n ",(unsigned int)tid ,gdata->thread_no,copydata->thread_no);

	}
	pthread_exit(0); 
}
