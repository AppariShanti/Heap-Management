#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define NUM_SEGS 6
int tag=1;
typedef enum{FALSE,TRUE} Boolean;
int lengths[]={4,4,8,8,10,12};
int varblock[]={40,40,40,48,48,48,64,64,64,128,128,256};
typedef struct heapElementTag
{
	struct heapElementTag* next;
	struct heapElementTag* prev;
	int size;
	int elmntID;
	Boolean isAllocated;
}heapElement;

heapElement* my_Heap[NUM_SEGS];           //Free Segregated List with 6 seglists
heapElement* allocList=NULL;              //Explicit allocated List


void createList(int block_size,int length,int idx)      //initialising all the seglists with sizes 2,4,8,16,32 
{
	heapElement* nptr=NULL;
	heapElement* lptr;
	int i;
	if(my_Heap[idx]==NULL)
	{
		lptr=(heapElement*)malloc(sizeof(heapElement));
		my_Heap[idx]=lptr;
		lptr->prev=my_Heap[idx];
		lptr->next=NULL;
		lptr->size=block_size;
		lptr->isAllocated=FALSE;
	}
	for(i=0;i<length-1;i++)
	{
		nptr=(heapElement*)malloc(sizeof(heapElement)); //what is the use of block size then???
		nptr->size=block_size;
		nptr->isAllocated=FALSE;
		nptr->next=NULL;
		lptr->next=nptr;
		nptr->prev=lptr;
		lptr=nptr;
	}
}
void endlist(int length,int idx)     //seglist with chunks in range 40-256
{
	heapElement* nptr=NULL;
	heapElement* lptr;
	int i,j=0;
	if(my_Heap[idx]==NULL)
	{
		lptr=(heapElement*)malloc(sizeof(heapElement));
		my_Heap[idx]=lptr;
		lptr->prev=my_Heap[idx]	;
		lptr->next=NULL;
		lptr->size=varblock[j++];
		lptr->elmntID=0;
		lptr->isAllocated=FALSE;
	}
	for(i=0;i<length-1;i++)
	{
		nptr=(heapElement*)malloc(sizeof(heapElement)); //what is the use of block size then???
		nptr->size=varblock[j++];
		nptr->elmntID=0;
		nptr->isAllocated=FALSE;
		nptr->next=NULL;
		lptr->next=nptr;
		nptr->prev=lptr;
		lptr=nptr;
		
	}
}
void initialiseHeap()                     //initialising all the free lists
{
	int i;
	for(i=0;i<NUM_SEGS-1;i++)
	{
		my_Heap[i]=NULL;
		createList(pow(2,i+1),lengths[i],i);
	}
	my_Heap[i]=NULL;
	endlist(lengths[i],i);
}
/* Add the required chunk in the FreeList to AllocatedList...
   This includes searching for the FIRST FIT ELEMENT in the free list and movig that chunk to allocated list*/
void addAlloc(heapElement** npptr)       
{
	heapElement* nptr=*npptr;
	if(allocList==NULL)
	{
		allocList=nptr;
		//printf("Added\n");
	}
	else
	{
		nptr->next=allocList;
		nptr->prev=NULL;
		allocList->prev=nptr;
		allocList=nptr;
	}
	*npptr=nptr;
}
heapElement* removeFree(heapElement** pptr,heapElement* lptr,int idx)
{
	heapElement* ptr=*pptr;
	if(lptr->prev==NULL && lptr->next==NULL)        
	{
		ptr->prev=NULL;
		ptr->next=NULL;
		lptr=NULL;
		printf("here");
	}
	else if(ptr->next!=NULL)
	{
		lptr=ptr->next;
		ptr->next->prev=lptr;
		ptr->next=NULL;
		ptr->prev=NULL;
		
	}
	else if(ptr->prev!=NULL && ptr->next!=NULL)
	{
		printf("here");
		ptr->next->prev=ptr->prev;
		ptr->prev->next=ptr->next;
		ptr->prev=NULL;
		ptr->next=NULL;
	}
	else
	{
		ptr->prev->next=NULL;
		ptr->prev=NULL;
	}
	*pptr=ptr;
	return lptr;
}
heapElement* my_alloc(int alloc_size)
{
	int i;
	Boolean found=FALSE;
	heapElement* lptr;
	for(i=0;i<NUM_SEGS-1 && !found;i++)
	{
		if(alloc_size<=my_Heap[i]->size)
		{
			lptr=my_Heap[i];
		    while(lptr!=NULL && !found)
		    {
		   		lptr->elmntID=tag; 	
				tag++;	
		    	my_Heap[i]=removeFree(&lptr,my_Heap[i],i);
		   		addAlloc(&lptr);
		   		found=TRUE;
			}
		}
	}
	if(found==FALSE)
	{
		lptr=my_Heap[NUM_SEGS-1];
		while(!found && lptr!=NULL)
		{
			if(alloc_size<=lptr->size)
			{
				if(lptr->isAllocated==FALSE)
				{

					lptr->isAllocated=TRUE;
					lptr->elmntID=tag++;
					
					my_Heap[NUM_SEGS-1]=removeFree(&lptr,my_Heap[NUM_SEGS-1],NUM_SEGS-1);
		    		addAlloc(&lptr);
					found=TRUE;
				}
				else
				{
					lptr=lptr->next;
				}
			}
			else
			lptr=lptr->next;
			
		}	
	}
	if(found==TRUE)
	{
		return lptr;
	}
	else
	{
		return NULL;
	}
}
void removeAlloc(heapElement** pptr)
{
	heapElement* ptr=*pptr;
	
	if(allocList==NULL)
	{
		printf("Sorry Nothing Allocated\n");
	}
	if(ptr->next==NULL && ptr->prev==NULL)
	{
		allocList=NULL;
	}
	if(ptr->next!=NULL && ptr->prev!=NULL)
	{
		ptr->prev->next=ptr->next;
		ptr->next->prev=ptr->prev;
	}
	else if(ptr->next!=NULL && ptr->prev==NULL)
	{
		allocList=ptr->next;
		allocList->prev=NULL;
	}
	else
	{
		ptr->prev->next=ptr->next;
		ptr->prev=NULL;
	}
	ptr->next=NULL;
	ptr->prev=NULL;
	*pptr=ptr;
}
void addFree(heapElement** pptr)
{
	Boolean found = FALSE;
	
	heapElement* ptr=*pptr;
	int i;
	for(i=0;i<NUM_SEGS-1 && !found;i++)
	{
		if(ptr->size==my_Heap[i]->size)
		{
			if(my_Heap[i]!=NULL)
			{
				ptr->next=my_Heap[i];
				my_Heap[i]->prev=ptr;
				my_Heap[i]=ptr;
			}
			else
			{
				my_Heap[i]=ptr;
				ptr->next=NULL;
				ptr->prev=NULL;
			}
			found=TRUE;
		}
	}
	if(found==FALSE)
	{
		i=NUM_SEGS-1;
		if(my_Heap[i]!=NULL)
			{
				ptr->next=my_Heap[i];
				my_Heap[i]->prev=ptr;
				my_Heap[i]=ptr;
			}
			else
			{
				my_Heap[i]=ptr;
				ptr->next=NULL;
				ptr->prev=NULL;
			}
	}
	*pptr=ptr;
	
}

void my_free(int id_tag)
{
	Boolean found=FALSE;
	heapElement* lptr=allocList;
	while(lptr!=NULL && !found)
	{
		if(lptr->elmntID==id_tag)
		{
			removeAlloc(&lptr);
			addFree(&lptr);
			found=TRUE;
		}
		else
		{
			lptr=lptr->next;
		}
	}
	if(found==FALSE)
	{
		printf("\nIncorrect Tag\n");
	}
}
void printlist()
{
	heapElement *lptr1,*lptr2,*lptr3,*lptr4,*lptr5,*lptr6;
	int i;
	lptr1=my_Heap[0];
	lptr2=my_Heap[1];
	lptr3=my_Heap[2];
	lptr4=my_Heap[3];
	lptr5=my_Heap[4];
	lptr6=my_Heap[5];
	while(lptr1!=NULL || lptr2!=NULL || lptr3!=NULL || lptr4!=NULL || lptr5!=NULL || lptr6!=NULL )
	{
		if(lptr1!=NULL)
		{
			printf("%d\t",lptr1->size);
			lptr1=lptr1->next;
		}
		else
		printf("\t");
		if(lptr2!=NULL)
		{
			printf("%d\t",lptr2->size);
			lptr2=lptr2->next;
		}
		else
		printf("\t");
		if(lptr3!=NULL)
		{
			printf("%d\t",lptr3->size);
			lptr3=lptr3->next;
		}
		else
		printf("\t");
		if(lptr4!=NULL)
		{
			printf("%d\t",lptr4->size);
			lptr4=lptr4->next;
		}
		else
		printf("\t");
		if(lptr5!=NULL)
		{
			printf("%d\t",lptr5->size);
			lptr5=lptr5->next;
		}
		else
		printf("\t");
		if(lptr6!=NULL)
		{
			printf("%d\t",lptr6->size);
			lptr6=lptr6->next;
		}
		else
		printf("\t");
		printf("\n");
	}
	printf("\n\n\n");
}
void printAlloc()
{
	heapElement* lptr=allocList;
	while(lptr!=NULL)
	{
		printf("%d------ %d\n",lptr->elmntID,lptr->size);
		lptr=lptr->next;
	}
	printf("\n\n");
}

int main()
{
	int i,j,choice=1;
	int size=0,tag_id=0;
	heapElement* nptr;
	initialiseHeap();
	printlist();
	printf("---------------MEMORY ALLOCATOR------------------\n\n");
	do
	{	printf("\n\n");
		printf("1: ALlocate Memory\n");
		printf("2: Deallocate Memory\n");
		printf("3: Display Free List\n");
		printf("4: Display Allocated List\n");
		printf("0: Exit\n\n");
		printf("Enter your choice : ");
		scanf("%d",&choice);
		switch(choice)
		{
			case 1: printf("Enter the size\n");
			        scanf("%d",&size);
			        nptr=my_alloc(size);
			        if(nptr==NULL)
					{
						printf("HEAP FULL!!!\n");
					}
					else
					{
						printf("\n\nYour tag is %d\n\n",nptr->elmntID);
					}
					break;
			case 2: printf("\n\nEnter Tag to delete\n\n");
					scanf("%d",&tag_id);
					my_free(tag_id);
					break;
			case 3: printlist();
					break;
			case 4: printAlloc();
					break;
			case 0: printf("EXITING\n\n");
					break;
			default: printf("INVALID ENTRY..TRY AGAIN\n\n");
		}
		
	}while(choice!=0);
}

