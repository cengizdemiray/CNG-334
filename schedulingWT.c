#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#define N 6
struct process // represents the attiribute of the process
{
    int id;
    int execution_time;
    int priority;

};
struct Queue // represents the queue of processes that has above attributes
{
    int capacity;
    int front;
    int rear;
    int size;
    struct process ptr[N];
};
struct thread_args // this is used for passing more than one parameter to threads
{
    struct Queue *queue;
    char t;
};
void initialize(struct Queue *);
struct process *createQueue();
void makeEmpty(struct Queue *);
int succ (int, struct Queue *q);
void enqueue (struct process, struct Queue *);
void* sortProcesses (void *);
void* executeProcesses (void *);
int isEmpty(struct Queue *);
int isFull (struct Queue *);
int main(void)
{
    struct thread_args *args = mmap(NULL, sizeof(struct thread_args), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0); // this is memory segment that is shared amon the child and parent process.
    printf("Enter the t:");
    scanf("%c",&args->t);
    args->queue = malloc(sizeof(struct Queue)); // allocation for queue of processes
    if(args->queue==NULL) // if allocation cannot be done give error message
    {
        printf("ERROR!");
        return -1;
    }
    makeEmpty(args->queue); // for making empty queue.
    initialize(args->queue); // for populating the queue
    pthread_t t1; // thread declaration
    pthread_t t2; // thread declaration
    pthread_create(&t1,NULL,sortProcesses,args ); // creation and definition of a thread
    pthread_join(t1,NULL); // until thread completes it's execution stop the main thread
    pthread_create(&t2,NULL,executeProcesses,args); // creation and definition of a thread
    pthread_join(t2,NULL); // until thread completes it's execution stop the main thread
    return 0;
}
int isFull(struct Queue *q) // for chechking whether the queue is full or not.
{
    if(q->size==q->capacity)
        return 1;
    return 0;
}
int isEmpty(struct Queue *q) // for checking whether the queue is empty or not.
{
    if(q->size==0)
        return 1;
    return 0;
}
void makeEmpty(struct Queue *q)
{
    q->size=0;
    q->front =0;
    q->rear=-1;
}
int succ (int value, struct Queue *q) // this is helper function when queue is populating
{
    // if value, that is the rear index equals to capacity go to head of the array
    {
        value++;
        if(value==q->capacity)
        {
            value =0;
        }
        return value;
    }
}
    void enqueue (struct process elements, struct Queue *q) // for adding the populated array elements to queue
    {
        if(isFull(q))
            printf("The queue is full !!");
        else
        {
            q->size++;
            q->rear = succ(q->rear,q);
            q->ptr[q->rear] = elements;
        }
    }
    void initialize(struct Queue *queue) // for populating the elements of queue and send them to enqueue function for addition of that node to queue
    {
        srand(time(NULL));
        struct process elements;
        queue->capacity = N;
        for (int i=0; i<N; i++)
        {
            elements.id = i+1;
            elements.execution_time = 10 + rand()%21;
            elements.priority = 1 + rand()%10;
            enqueue(elements, queue);
        }
    }
    void* sortProcesses(void *args) // used for sorting process according to synchronization algorithm.
    {
        struct thread_args *threadArgs = args; // type conversion, so that we can use our passed parameters inside the function
        int i,j,minindex;
        struct process temp; // temporary only one process
        if(threadArgs->t == 'P') // if algorithm is done according to priority execute this block
        { // if Priority scheduling will be applied, selection sort need to be done here
            if(isEmpty(threadArgs->queue))
            {
                printf("Queue is empty!");
            }else
            {
                for(i=threadArgs->queue->front; i<=threadArgs->queue->rear; i++)
                {
                    minindex = i;
                    for(j=i+1; j<= threadArgs->queue->rear; j++)
                    {
                        if (threadArgs->queue->ptr[j].priority <threadArgs->queue->ptr[minindex].priority)
                        {
                            minindex = j;
                        }
                    }
                    temp = threadArgs->queue->ptr[minindex];
                    threadArgs->queue->ptr[minindex] = threadArgs->queue->ptr[i];
                    threadArgs->queue->ptr[i] = temp;
                }
            }
        }else if(threadArgs->t=='E') // if scheduling is done according to execution time, insertion sort will be applied.
        {
            if(isEmpty(threadArgs->queue))
            {
                printf("Queue is empty!");
            }
            else
            {
                for(i = threadArgs->queue->front + 1; i <= threadArgs->queue->rear; i++)
                {
                    temp = threadArgs->queue->ptr[i];
                    j = i - 1;
                    while(j >= threadArgs->queue->front && threadArgs->queue->ptr[j].execution_time > temp.execution_time)
                    {
                        threadArgs->queue->ptr[j + 1] = threadArgs->queue->ptr[j];
                        j = j - 1;
                    }
                    threadArgs->queue->ptr[j + 1] = temp;
                }
            }
        }else
        {
            printf("Invalid!!");
        }
        return NULL;
    }
    void* executeProcesses (void* args) // this is used for printing execution of order based on the selected scheduling algorithm.
    {
    struct thread_args *threadArgs = args; // type conversion, so that we can use our passed parameters inside the function
        if(threadArgs->t == 'P')
        {
            printf("The synchoranazation is done according to Priority\n");
        }else
        {
            printf("The synchronazation is done according to Execution time\n");
        }
        while (!isEmpty(threadArgs->queue)) // untill all of the elements dequeued, take the elements seperately and prints
        {
            printf("İd:%d Priority:%d Execution Time:%d\n ",threadArgs->queue->ptr[threadArgs->queue->front].id,threadArgs->queue->ptr[threadArgs->queue->front].priority,threadArgs->queue->ptr[threadArgs->queue->front].execution_time);
            threadArgs->queue->front++;
            threadArgs->queue->size--;
        }
    }

