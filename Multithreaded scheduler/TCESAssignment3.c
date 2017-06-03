/*

	Jonathan Chac, Reid Stuberg
	Assignment 3
	*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define CPU 1
#define IO 2
#define NR_PHASES 2

//Create the locks
pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
//Globals
int NR_JOBS =0;
int jobID = 0;
// Job Structure
struct job {
	int job_id;
	int nr_phases;
	int jobTime;
    int current_phase;
	int phasetype_and_duration[NR_PHASES][NR_PHASES];
	int is_completed;
}; 
// Node that holds job and it's next node
struct QNode
{
    struct job *currentJob;
    struct QNode *next;
};
// holds the front and rear node and size of queue
struct Queue
{
    struct QNode *front, *rear;
	int size;
};
//creates the node that holds job struct.
struct QNode* newNode(struct job *j)
{
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->currentJob = j;
    temp->next = NULL;
    return temp; 
}
// creates the queue
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}
// enqueues job to queue
void enQueue(struct Queue *q,struct job* k)
{
    struct QNode *temp = newNode(k);
    if (q->rear == NULL)
    {
       q->front = q->rear = temp;
       return;
    }
    q->rear->next = temp;
    q->rear = temp;
	q->rear->next = NULL;
	q->size++;
}
 
// removes a job from the queue
struct QNode *deQueue(struct Queue *q)
{
	if (q->front == NULL)
       return NULL;
    struct QNode *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL){
		 q->rear = NULL;
	}
    q->size--;
    return temp;
}
// Global queues Cpu, io, and finished
struct Queue* cpuQ;
struct Queue* ioQ;
struct Queue* fQ;


void *JobCreation()
{
	while (1){
		pthread_mutex_lock( &mutex0 );
		int k;
		struct job *j = (struct job*)malloc(sizeof(struct job));
			j->job_id = jobID;
			j->current_phase = 0;
			j->nr_phases = NR_PHASES;

			for (k = 0; k < NR_PHASES; k++){
				j->phasetype_and_duration[0][k] = (rand()%2);
				j->phasetype_and_duration[1][k] = (rand()%5);
			}
			jobID++;
			printf("Job CREATED %d\n", j->job_id);
			enQueue(cpuQ,j);
			printf("Job %d is ENTERING CPU QUEUE \n",j->job_id);
			pthread_mutex_lock( &mutex3 );
			if(fQ->size>0){
				struct QNode *temp = deQueue(fQ);
				printf("Job %d dequeueing off FinalQ\n ", temp->currentJob->job_id);
				NR_JOBS++;
				free(temp->currentJob);
				free(temp);
			}
			pthread_mutex_unlock( &mutex3 );
			sleep(2);
			pthread_mutex_unlock( &mutex0 );
	}
}
void *CpuProcess(){
	while(1){
		pthread_mutex_lock( &mutex1 );
		if(cpuQ->size != 0){
			
			struct QNode* temp = deQueue(cpuQ);
			int currPhase =temp->currentJob->current_phase;
			if(currPhase == (NR_PHASES-1)){
				pthread_mutex_lock( &mutex3 );
				printf("Job %d entering Finished queue \n",temp->currentJob->job_id);
				enQueue(fQ,temp->currentJob);
				pthread_mutex_unlock( &mutex3 );
			}	
			else{
				int phaseType =temp->currentJob->phasetype_and_duration[0][currPhase];
				if(phaseType == 1){
						
					int waitTime = temp->currentJob->phasetype_and_duration[1][currPhase];
					printf("Waiting for JOB %d on CPU\n",temp->currentJob->job_id );
					sleep(waitTime);
					temp->currentJob->current_phase++;
					enQueue(cpuQ,temp->currentJob);
						
				}
				if(phaseType == 2){
					pthread_mutex_lock( &mutex2 );
					enQueue(ioQ,temp->currentJob);
					printf("Job %d entering ioQueue \n",temp->currentJob->job_id);
					pthread_mutex_unlock( &mutex2 );
				}
				
			}
			free(temp);
			pthread_mutex_unlock( &mutex1 );	
		}
		pthread_mutex_unlock( &mutex1 );
	}
}
void *IOProcess(){
	while (1){
		pthread_mutex_lock( &mutex2 );
		if ( ioQ->size > 0){
			struct QNode *temp=deQueue(ioQ);
			int dur;
			int currPhase = temp->currentJob->current_phase;
			dur = temp->currentJob->phasetype_and_duration[1][currPhase];
			printf("Waiting for JOB %d on IO\n",temp->currentJob->job_id );
			sleep(dur);
			temp->currentJob->current_phase++;
			pthread_mutex_lock( &mutex1 );
			enQueue(cpuQ,temp->currentJob);
			printf("Job %d entering back to CPU",temp->currentJob->job_id);
			free(temp);
			pthread_mutex_unlock( &mutex1 );
			
		}
		pthread_mutex_unlock( &mutex2 );
	}
}

int main()
{	
	int i;
	// Initialize the Locks
	pthread_mutex_init (&mutex0,NULL);
	pthread_mutex_init (&mutex1,NULL);
	pthread_mutex_init (&mutex2,NULL);
	pthread_mutex_init (&mutex3,NULL);
	//Makes the global queues
	cpuQ = createQueue();
	ioQ = createQueue();
	fQ = createQueue();
	//Initialize the size of queue
	cpuQ->size = 0;
	ioQ->size = 0;
	fQ->size = 0;	
	//Create the threads
    pthread_t t[16];

	//CPU threads
    for (i = 0; i < 8; i++){
        pthread_create(&t[i], NULL, CpuProcess, NULL);
    }//IO Threads
	for (; i < 12; i++){
        pthread_create(&t[i], NULL, IOProcess, NULL);
    }//Job Threads
	for (; i < 16; i++){
        pthread_create(&t[i], NULL, JobCreation, NULL);
    }
	// Joins all the threads
    for (i = 0; i < 16; i++){
        pthread_join(t[i], NULL);
    }
	
    return 0;
}
