thread:
	gcc thread.c -o produce -lm -lpthread

process:
	gcc consumer.c -lm -lrt -o consumer
	gcc producer.c -lm -lrt -o producer
	gcc process.c -lm -lrt -o produce