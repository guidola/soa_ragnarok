#include "roundRobin.h"
#include <stdio.h>

RoundRobin RR_create(long size){

	RoundRobin r;

	r.data = (RR_element*) malloc(sizeof(RR_element) * (size + 1));
	r.first = 0;
	r.last = 0;
	r.size = size + 1;

	return r;
}


int RR_push_fd(RoundRobin* r, int fd){

	if(r->last == r->first - 1 || (r->last == r->size - 1 && r->first == 0)) return 0;

	r->data[r->last].fd = fd;
	r->last = (r->last + 1) % r->size;
	//printf("r de last is %li\n", r->last);

	return 1;
}


int RR_push(RoundRobin* r, void* e){

	if(r->last == r->first - 1 || (r->last == r->size - 1 && r->first == 0)) return 0;

	switch(sizeof((*e))){

		case sizeof(struct sockaddr_in):
			//printf("rr socket\n");
			r->data[r->last].s_addr = *((struct sockaddr_in*)e);
			break;
		case sizeof(pthread_t):
			//printf("rr thread\n");
			r->data[r->last].thid = *((pthread_t*)e);
			break;
		case sizeof(int):
			//printf("rr fd\n");
			r->data[r->last].fd = *((int*)e);
			break;
		default:break;
	}

	r->last = (r->last + 1) % r->size;
	//printf("r de last is %li", r->last);

	return 1;
}


RR_element RR_pull(RoundRobin* r){

	RR_element retval;

	if(r->first == r->last){
		retval.fd = -1;
		return retval;
	}

	retval = r->data[r->first];
	r->first = (r->first + 1) % r->size;

	return retval;
}

pthread_t RR_pull_pthread_t(RoundRobin* r){

	if(r->first == r->last) return 0;

	pthread_t retval;
	retval = r->data[r->first].thid;
	r->first = (r->first + 1) % r->size;

	return retval;
}

struct sockaddr_in RR_pull_sockadrr_in(RoundRobin* r){

	struct sockaddr_in retval;

	if(r->first == r->last) return retval;

	retval = r->data[r->first].s_addr;
	r->first = (r->first + 1) % r->size;

	return retval;
}

int RR_pull_fd(RoundRobin* r){

	if(r->first == r->last) return -1;

	int retval;
	retval = r->data[r->first].fd;
	r->first = (r->first + 1) % r->size;

	return retval;
}


int RR_isEmpty(RoundRobin r){
	//printf("first %li i last %li!\n", r.first, r.last);
	return r.first == r.last;
}

int RR_copy(RoundRobin* dst, RoundRobin* src){

	memcpy(dst->data, src->data, sizeof(RR_element) * src->size);

	dst->first = src->first;
	dst->last = src->last;
	dst->size = src->size;

	return 1;
}


int RR_destroy(RoundRobin* r){

	free(r->data);
	r->size = r->first = r->last = 0;

	return 1;
}

int RR_delete(RoundRobin* r, void* e){

	if(r->first == r->last) return 0;

	int i;
	i = r->first;

	while(i != r->last){

		if(r->data[i].fd == *((int*)(e))){
			break;
		}

		i = (i + 1) % (r->size);

	}

	if(i == r->last){
		if(r->data[i].fd != *((int*)(e))) return 0;
	}

	if(i > r->last){
		memmove (&(r->data[r->last + 1]), &(r->data[r->last]), i - r->last);
		(r->last)++;
	}else if(i < r->first){
		memmove (&(r->data[i]), &(r->data[i + 1]), r->first - i);
		(r->first)--;
	}

	r->last = (r->last + 1) % r->size;

	return 1;
}

int* RR_getAll(RoundRobin r) {

	long n_elems;

	if (r.first == r.last) {
		n_elems = 0;
	} else if (r.first > r.last) {
		n_elems = r.size - r.first + r.last;
	} else {
		n_elems = r.last - r.first;
	}

	int * values = (int*)calloc(n_elems, sizeof(int));
	long i, k = 0;

	for ( i = r.first; i < n_elems; i = (i + 1) % r.size) {
		values[k++] = r.data[i].fd;
	}

	return values;
}
