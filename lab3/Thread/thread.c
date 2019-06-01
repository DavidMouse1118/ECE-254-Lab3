#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <math.h>

int N,B,P,C;
int total_consumed;
int *sharedbuffer;
int bufferCount;
//Declare all the semaphore
sem_t spaces, items, lock;

void *producer(void *arg) {
	int producerId;
	int count = 0;

	producerId = *(int *)arg;
	free(arg);

	while(1) {
		//Produce item
		int item = producerId + count * P;
		if(item >= N) {
			break;
		}
		//Wait for spaces
		sem_wait(&spaces);
		//Lock the modification
		sem_wait(&lock);
		//Add item
		sharedbuffer[bufferCount] = item;
		bufferCount++;
		count++;
		//signal lock
		sem_post(&lock);
		//signal items
		sem_post(&items);
	}

	pthread_exit(0);
}

void *consumer(void *arg) {
	int consumerId;
	int item;
	int itemSqrt;

	consumerId = *(int *)arg;
	free(arg);

	while(total_consumed < N) {
		sem_wait(&items);
		sem_wait(&lock);
		//pop
		if(bufferCount <= 0) {
			sem_post(&lock);
			sem_post(&items);
			break;
		}
		bufferCount--;
		item = sharedbuffer[bufferCount];
		total_consumed++;
		sem_post(&lock);
		sem_post(&spaces);
		//Signal other consumer
		if (total_consumed >= N){
			sem_post(&items);
		}
		//Consume item
		itemSqrt = sqrt(item);
		if(itemSqrt * itemSqrt == item) {
			printf("%d %d %d \n", consumerId, item, itemSqrt);
		}
	}

	pthread_exit(0);
}

int main(int argc, const char * argv[]){
	if (argc < 5) {
		printf("Incorrect number of arguments.\n");
		exit(1);
	}
	N = atoi(argv[1]);
	B = atoi(argv[2]);
	P = atoi(argv[3]);
	C = atoi(argv[4]);
    
	total_consumed = 0;

	//Init shared buffer using array
	sharedbuffer = (int *) malloc(sizeof(int) *B);

	//Init semaphore
	sem_init(&spaces, 0, B);
	sem_init(&items, 0, 0);
	sem_init(&lock, 0, 1);

	pthread_t *producer_t = malloc(sizeof(pthread_t) * P);
	pthread_t *consumer_t = malloc(sizeof(pthread_t) * C);

	struct timeval tv;
	double t1;
	double t2;

	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;

	int i;
	for (i = 0; i < P; i++) {
		int *id = malloc(sizeof(int));
		*id = i;
		pthread_create(&producer_t[i], NULL, producer, id);
	}

	for (i = 0; i < C; i++) {
		int *id = malloc(sizeof(int));
		*id = i;
		pthread_create(&consumer_t[i], NULL, consumer, id);
	}

	for (i = 0; i < P; i++) {
		pthread_join(producer_t[i], NULL);
	}

	for (i = 0; i < C; i++) {
		pthread_join(consumer_t[i], NULL);
	}

	gettimeofday(&tv, NULL);
	t2 = tv.tv_sec + tv.tv_usec/1000000.0;

	printf("System execution time: %.6lf seconds elapsed.\n", t2-t1);

	//Clear up
	free(producer_t);
	free(consumer_t);
	free(sharedbuffer);
	sem_destroy(&items);
	sem_destroy(&spaces);
	sem_destroy(&lock);

	return 0;
}