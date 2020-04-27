#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include <string.h>


pthread_mutex_t conveyorMutex;
pthread_cond_t	empty	= PTHREAD_COND_INITIALIZER;
pthread_cond_t	full	= PTHREAD_COND_INITIALIZER;


struct brick{
    int brickNumber;
    int weight;
};

int conveyorLoadNow=0;
int whichBrick=0;
struct brick bricksArray[10];
int countBricks=0;

void *putBrick( void *arg)
{
    while(1) {
        srand(time(NULL)+*(int*)arg);

        int weightOfThatBrick=rand() % 2 + 1;
        pthread_mutex_lock(&conveyorMutex);
        while(conveyorLoadNow + weightOfThatBrick > 10) {
            pthread_cond_wait(&full, &conveyorMutex);

        }

        bricksArray[countBricks].weight =weightOfThatBrick;
        bricksArray[countBricks].brickNumber=whichBrick++;
        conveyorLoadNow += bricksArray[countBricks].weight;
        printf(" worker %d put a brick on the conveyor, there is now %d kg on conveyor \n",*(int*)arg,
               conveyorLoadNow);
        countBricks++;
        sleep(2);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&conveyorMutex);
	sleep(1);


    }
}

void *takeBrick(void * arg)
{
    while(1) {

        pthread_mutex_lock(&conveyorMutex);
        if(conveyorLoadNow==0) {
            pthread_cond_wait(&empty, &conveyorMutex);
            }
        conveyorLoadNow -=  bricksArray[--countBricks].weight;
        printf("worker %d took a brick, there is now %d kg on conveyor \n", *(int*)arg, conveyorLoadNow);
        sleep(2);

        pthread_cond_signal(&full);


        pthread_mutex_unlock(&conveyorMutex);
	sleep(1);


    }
    }

int main() {
    const int numberOfWorkersOnEnd=3;
    const int NumberOfWorkersOnStart=3;
    const int n=NumberOfWorkersOnStart+numberOfWorkersOnEnd;
    int WorkerID[n];
    pthread_t workersEnd[numberOfWorkersOnEnd];
    pthread_t workersStart[NumberOfWorkersOnStart];

    pthread_mutex_init(&conveyorMutex,NULL);
    int end=0,start=0,id=0;

    while(end<numberOfWorkersOnEnd|| start<NumberOfWorkersOnStart){

        if(end<numberOfWorkersOnEnd){
            WorkerID[id]=id;
            pthread_create(&workersEnd[end], NULL,takeBrick , &WorkerID[id]);
            printf("Created worker number:%d\n",id);
	    id++;
            end++;
        }

        if(start<NumberOfWorkersOnStart){
            WorkerID[id]=id;
            pthread_create(&workersStart[start], NULL, putBrick,&WorkerID[id]);
             printf("Created worker number:%d\n",id);
	     id++;
            start++;
        }
    }

    for(int i=0;i<numberOfWorkersOnEnd;i++) {
        pthread_join(workersEnd[i], NULL);
    }
    for(int i=0;i<NumberOfWorkersOnStart;i++) {
        pthread_join(workersStart[i], NULL);
    }
    return 0;
}

