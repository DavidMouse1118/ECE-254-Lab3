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

int main (int argc, char *argv[]) {

	int itemNum, producerNum, consumerNum, bufferSize, consumerId;

	itemNum = atoi(argv[1]);
	consumerId = atoi(argv[2]);
	producerNum = atoi(argv[3]);
	consumerNum = atoi(argv[4]);

	mqd_t mq;
	char *name = "/MessageQueue_z498zhan";

	//Open the message queue
	mq = mq_open(name, O_RDONLY);
	if (mq == -1 ) {
		perror("mq open failed.\n");
		exit(1);
	}

	mqd_t cq;
	char *cname = "/CountQueue_z498zhan";

	//Open the count queue for counting the recieved items
	cq = mq_open(cname, O_RDWR);
	if (cq == -1 ) {
		perror("cq open failed.\n");
		exit(1);
	}

	int item;
	int itemSqrt;
	int received_count;
	while(true) {

		//Recieve count from cq
		if(mq_receive(cq, (char *)&received_count, sizeof(int), 0) == -1){
			perror("Failed receiving item from cq\n");
			return 1;
		}

		//if all item are recieved, signal other process and break
		if(received_count == itemNum){
			if(mq_send(cq, (char *)&received_count, sizeof(int), 0) == -1){
				perror("Failed sending item to cq\n");
			}
			break;
		}

		//increment received_count
		if (received_count < itemNum){
			received_count++;
			if(mq_send(cq, (char *)&received_count, sizeof(int), 0) == -1){
				perror("Failed sending item to cq\n");
			}
		}

		//Recieve item from message queue
		if(mq_receive(mq, (char *)&item, sizeof(int), 0) == -1){
			perror("Failed receiving item from mq\n");
			return 1;
		}
		
		//Consume item
		itemSqrt = sqrt(item);
		if(itemSqrt * itemSqrt == item) {
			printf("%d %d %d \n", consumerId, item, itemSqrt);
		}

	}

	if (mq_close(mq) == -1) {
		perror("mq failed to close");
		exit(2);
	}

	if (mq_close(cq) == -1) {
		perror("cq failed to close");
		exit(2);
	}

	return 0;
}