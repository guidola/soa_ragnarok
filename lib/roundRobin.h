#ifndef ROUNDROBIN_H_
#define ROUNDROBIN_H_

#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

typedef union{

	struct sockaddr_in s_addr;
	pthread_t thid;
	int fd;

}RR_element;

typedef struct{

	RR_element* data;
	long first;
	long last;
	long size;
}rr;

typedef rr RoundRobin;

RoundRobin RR_create(long size);
int RR_push(RoundRobin* r, void* e);
RR_element RR_pull(RoundRobin* r);
pthread_t RR_pull_pthread_t(RoundRobin* r);
struct sockaddr_in RR_pull_sockadrr_in(RoundRobin* r);
int RR_pull_fd(RoundRobin* r);
int RR_isEmpty(RoundRobin r);
int RR_destroy(RoundRobin* r);
int RR_copy(RoundRobin* dst, RoundRobin* src);
int RR_delete(RoundRobin* r, void* e);
int RR_push_fd(RoundRobin* r, int fd);


#endif //ROUNDROBIN_H_
