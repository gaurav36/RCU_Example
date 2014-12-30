#include <stdio.h>      
#include <pthread.h>    
#include <string.h>
#include <urcu-bp.h>  
#include <urcu/rculist.h>
#include <pthread.h>


CDS_LIST_HEAD(ghead);

void list_adder( void *data);
void list_remover(void *data);
void list_printer (void *data);


struct mynode
{
	int value;
	struct cds_list_head link;
//	struct rcu_head rcu_head;	/* for call_rcu() */
};

void list_printer (void *data)
{
	struct mynode *iter = NULL;

	while (1) 
	{
		rcu_read_lock();

		printf("\n\n\nList is ......\n\n");
		cds_list_for_each_entry_rcu (iter, &ghead, link) 
		{
			printf ("value = %d\n", iter->value);
		}
		sleep(10);
		
		rcu_read_unlock();
	}

}

void  list_adder( void *data)
{
	struct mynode *node;
	int val=1;


	while(1)
	{

		node = calloc(sizeof(*node), 1);
		if (!node)
			return ;

		node->value = val++;
		cds_list_add_rcu(&node->link, &ghead);
		printf("\none more node added to list with val =%d",node->value );
		sleep(11);

	}
	return ;
}

void list_remover(void *data)
{
	struct mynode *node,*n;

	while(1)
	{
		cds_list_for_each_entry_safe(node, n, &ghead, link) 
		{
			printf("\n\n>>>>>>>>Press enter key to delete  node \n\n");
			getchar();

			cds_list_del_rcu(&node->link);
			synchronize_rcu();
			printf("\n\n>>>>>>>>>>node is deleted which has data =%d\n\n",node->value );
			free(node);
		}

	}

}


int main ()
{
	pthread_t thread1, thread2, thread3;  

	pthread_create (&thread1, NULL, (void *) list_adder, NULL);
	pthread_create (&thread2, NULL, (void *) list_remover, NULL);
	pthread_create (&thread3, NULL, (void *) list_printer,NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
        return 0;
}
