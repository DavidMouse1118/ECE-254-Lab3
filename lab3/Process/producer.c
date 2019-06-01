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

	int itemNum, producerNum, consumerNum, bufferSize, producerId;

	itemNum = atoi(argv[1]);
	producerId = atoi(argv[2]);
	producerNum = atoi(argv[3]);
	consumerNum = atoi(argv[4]);

	mqd_t mq;
	char  *name = "/MessageQueue_z498zhan";
	//Open the message queue
	mq  = mq_open(name, O_RDWR);
	if (mq == -1 ) {
		perror("mq failed to open");
		exit(1);
	}

	int count = 0;

	while(1) {
		//Produce item
		int item = producerId + count * producerNum;
		if(item >= itemNum) {
			break;
		}
		//send item
		if(mq_send(mq, (char*)&item, sizeof(int), 0) == -1){
			perror("Sending item to mq failed\n");
		}
		count++;
	}

	if (mq_close(mq) == -1){
		perror("mq failed to close");
		exit(2);
	}

	return 0;
}