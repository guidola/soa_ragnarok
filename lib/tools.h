#ifndef TOOLS_H_
#define TOOLS_H_

#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "types.h"

#define N_CSV_COLUMNS 8


void* nothing(void);
void sfork(fn_ptr parentRutine, fn_ptr childRutine, void* pidqid);
pthread_t launchThread(fn_ptr func, void* param);
void* waitt(pthread_t thr);
double TimeSpecDiff(struct timespec *ts1, struct timespec *ts2);
void toLowerCase(char* cadena);
void signalHandler(int signal);
void print_mem(void const *vp, size_t n);
char* dtoa(char *s, double n);
char*  itoa(int n, char* s);
void reverse(char s[]);
char* ftoa(float n, char *res, int afterpoint);
void CSV_close(int fd);
void CSV_export(int fd, char data[N_CSV_COLUMNS][100], unsigned int n_elems);
int CSV_open(char* filename, char headers[N_CSV_COLUMNS][100], unsigned int n_columns);
float promediate(float * values, int nvalues);

#endif