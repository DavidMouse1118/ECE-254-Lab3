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

int newProcess(char* program, char** arguments) {
	pid_t pid;
	pid = fork();

	if(pid < 0) {
		fprintf (stderr, "an error occurred in fork\n");
		abort ();
	}
	else if (pid == 0){
		//Child process
		//Replace the current child program with program
		execvp (program, arguments);
		//The execvp function returns only if an error occurs.
		fprintf (stderr, "an error occurred in execvp\n");
		abort();
	}
	else {
		//Parent process
		return pid;
	}
}




int main(int argc, char * argv[]) {
	if (argc < 5) {
		printf("Incorrect number of arguments. \n");
		exit(1);
	}

	N = atoi(argv[1]);
	B = atoi(argv[2]);
	P = atoi(argv[3]);
	C = atoi(argv[4]);

	//Initialize message queue
	mqd_t mq;
	char *name = "/MessageQueue_z498zhan";
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof( int );
	attr.mq_flags   = 0;

	mq_unlink(name);

	mq  = mq_open(name, O_RDWR | O_CREAT, mode, &attr);
	if (mq == -1 ) {
		perror("mq open failed");
		exit(1);
	}

	//Initialize count queue
	mqd_t cq;
	char * cname = "/CountQueue_z498zhan";
	mode_t mode_c = S_IRUSR | S_IWUSR;
	struct mq_attr c_attr;

	c_attr.mq_maxmsg = 1;
	c_attr.mq_msgsize = sizeof(int);
	c_attr.mq_flags = 0;


	mq_unlink(cname);
	
	cq = mq_open(cname, O_RDWR | O_CREAT, mode_c, &c_attr);
	if (cq == -1){
		perror("cq failed to open");
		exit(1);
	}

	int received_count = 0;
	if(mq_send(cq, (char*)&received_count, sizeof(int), 0) == -1){
		perror("cq failed to send");
	}

	struct timeval tv;
	double t1;
	double t2;

	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;

	int i, j;
	for(i = 0; i < P; i++) {
		int length = snprintf( NULL, 0, "%d", i );
		char id_str[length+1];
		sprintf(id_str, "%d", i);
		argv[2] = id_str;
		newProcess("./producer", argv);
	}

	for(j = 0; j < C; j++) {
		int length = snprintf( NULL, 0, "%d", j );
		char id_str[length+1];
		sprintf(id_str, "%d", j);
		argv[2] = id_str;
		newProcess("./consumer", argv);
	}

	int status = 0;
	//wait until all child processes return
	while (wait(&status) > 0);

	gettimeofday(&tv, NULL);
	t2 = tv.tv_sec + tv.tv_usec/1000000.0;

	printf("System execution time: %.6lf seconds elapsed.\n", t2-t1);

	if(mq_close(mq) == -1){
		perror("mq failed to close");
		exit(2);
	}

	if(mq_unlink(name) != 0) {
		perror("mq failed to unlink");
		exit(3);
	}

	 if(mq_close(cq) == -1){
	 	perror("cq failed to close");
		exit(2);
	 }

	 if(mq_unlink(cname) != 0){
	 	perror("cq failed to unlink");
	 	exit(3);
	 }

	return 0;



}