#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#define N 6
struct process // this is attributes of one one processes
{
    int id;
    int execution_time;
    int priority;

};
struct Queue //this is queue structure for an array of processes
{
    int capacity;
    int front;
    int rear;
    int size;
    struct process ptr[N];
};
void initialize(struct Queue *);
struct process *createQueue();
void makeEmpty(struct Queue *);
int succ (int, struct Queue *q);
void enqueue (struct process, struct Queue *);
void sortProcesses (struct Queue*,char);
void executeProcesses (struct Queue *, char);
int isEmpty(struct Queue *);
int isFull (struct Queue *);
int main(void)
{
    char t;
    printf("Enter the t:");
    scanf("%c",&t);
    struct Queue *queue = mmap(NULL, sizeof(struct Queue), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0); // this is the memory segment that is shared among parent and child processes.
    if(queue==NULL) // if it cannot be allocated successfully, give error.
    {
        printf("ERROR!");
        return -1;
    }
    makeEmpty(queue); // for creating empty queue
    initialize(queue); // for populating the queue
    pid_t pid = fork(); // for creating the child process
    if(pid==0) // if pid is 0, the child process is executing
    {
        sortProcesses(queue,t);
    }
    else
    {
        wait(NULL); // if it is not zero, stop the parent process, and wait for finishing the child process.
        executeProcesses(queue,t); // if child process is finished, continue to execute parent process with this function
    }
    return 0;
}
int isFull(struct Queue *q) // for controlling the whether queue is full or not
{
    if(q->size==q->capacity)
        return 1;
    return 0;
}
int isEmpty(struct Queue *q) // for controlling the whether queue is empty or not.
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
{ // if value, that is the rear index equals to capacity go to head of the array
    value++;
    if(value==q->capacity)
    {
        value =0;
    }
    return value;
}
void enqueue (struct process elements, struct Queue *q) // this is used for adding element to queue.
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
void initialize(struct Queue *queue) // this is used for entering the details and populating  of queue array.
{
    srand(time(NULL));
    struct process elements;
    queue->capacity = N;
    for (int i=0; i<N; i++)
    {
        elements.id = i+1;
        elements.execution_time = 10 + rand()%21; // random number generation between 10 and 30 and assign it to execution time of array element
        elements.priority = 1 + rand()%10; // random number generation between 1 and 10 and assign it to priority of array element.
        enqueue(elements, queue); // for adding this array element to queue
    }
}
void sortProcesses(struct Queue *q, char t) // used for sorting process according to synchronization algorithm.
{
    int i,j,minindex;
    struct process temp;
    if(t == 'P') // if character is P this means synchronization is done according to selection sort algorithm
    {
        if(isEmpty(q))
        {
            printf("Queue is empty!");
        }else
        {
            for(i=q->front; i<=q->rear; i++)
            {
               minindex = i;
               for(j=i+1; j<= q->rear; j++)
               {
                   if (q->ptr[j].priority < q->ptr[minindex].priority)
                   {
                       minindex = j;
                   }
               }
                temp = q->ptr[minindex];
                q->ptr[minindex] = q->ptr[i];
                q->ptr[i] = temp;
            }
        }
    }else if(t=='E') // if character is E, sort is done according to insertion sort.
    {
        if(isEmpty(q))
        {
            printf("Queue is empty!");
        }
        else
        {
            for(i = q->front + 1; i <= q->rear; i++)
            {
                temp = q->ptr[i];
                j = i - 1;
                while(j >= q->front && q->ptr[j].execution_time > temp.execution_time)
                {
                    q->ptr[j + 1] = q->ptr[j];
                    j = j - 1;
                }
                q->ptr[j + 1] = temp;
            }
        }
    }else
    {
        printf("Invalid!!");
    }
}
void executeProcesses (struct Queue *q, char t) // this is for printing the execution order based on the selected synchronization algorithm
{
    if(t == 'P')
    {
        printf("The synchoranazation is done according to Priority\n");
    }else
    {
        printf("The synchronazation is done according to Execution time\n");
    }
    while (!isEmpty(q)) // untill all of the elements dequeued, take the elements seperately and prints
    {
        printf("İd:%d Priority:%d Execution Time:%d\n ",q->ptr[q->front].id,q->ptr[q->front].priority,q->ptr[q->front].execution_time);
        q->front++;
        q->size--;
    }
}