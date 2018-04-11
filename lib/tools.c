#include "../common/typesNdConstants.h"
#include <math.h>


void* nothing(void){
	return NULL;
}

void sfork(fn_ptr parentRutine, fn_ptr childRutine, void* pidqid){

	switch(fork()){
		case -1:
			perror("Forking error");
			exit(EXIT_FAILURE);
			break;
		case 0:
			childRutine(pidqid);
			break;
		default:
			parentRutine(NULL);
	}

}

pthread_t launchThread(fn_ptr func, void* param){

	pthread_t pth;

	if(0 != pthread_create(&pth, NULL, func, param)){
		perror("Thread creation error");
		return -1;
	}

	return pth;
}

void* waitt(pthread_t thr){

	void* retvar = NULL;

	if(pthread_join(thr, &retvar)){
		perror("Error while waiting thread");
		return NULL;
	}
	return retvar;
}


void signalHandler(int signal){

	switch(signal){

		case SIGUSR1:
			//printf("SIGUSR1 received!!\n");
			break;
	}

}


#define NANO_SECONDS_IN_SEC 1000000000
/* returns a static buffer of struct timespec with the time difference of ts1 and ts2
   ts1 is assumed to be greater than ts2 */
double TimeSpecDiff(struct timespec *ts1, struct timespec *ts2){

	struct timespec ts;

	ts.tv_sec = ts1->tv_sec - ts2->tv_sec;
	ts.tv_nsec = ts1->tv_nsec - ts2->tv_nsec;

	if (ts.tv_nsec < 0) {
		ts.tv_sec--;
		ts.tv_nsec += NANO_SECONDS_IN_SEC;
	}

	return ts.tv_sec + ((double)(ts.tv_nsec) / NANO_SECONDS_IN_SEC);
}


void toLowerCase(char* cadena){

	int i;

	for(i = 0 ; cadena[i] != '\0' ; i++){
		if(cadena[i] >= 'A' && cadena[i] <= 'Z') cadena[i] += 32;
	}

}


//prints on hexa the information on a given opaque structure
void print_mem(void const *vp, size_t n) {

	unsigned char const *p = vp;
	size_t i;

	for (i=0; i<n; i++)
		printf("%02x\n", p[i]);
	putchar('\n');

}



//opens csv file
int CSV_open(char* filename, char headers[N_CSV_COLUMNS][100], unsigned int n_columns){
	int fd = open(filename, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG);
	char cadena[1000] = "";
	int i;

	for(i = 0 ; i < n_columns ; i++){

		strcat(cadena, "\"");
		strcat(cadena, headers[i]);
		strcat(cadena, "\",");
	}

	cadena[strlen(cadena) - 1] = '\n';
	write(fd, cadena, strlen(cadena));

	return fd;
}

#define N_CSV_COLUMNS 8
//exports given array into csv format row. data must be formatted to ASCII strings to be properly handled
void CSV_export(int fd, char data[N_CSV_COLUMNS][100], unsigned int n_elems){

	int i;
	char cadena[1000] = "";

	for( i = 0 ; i < n_elems ; i++){
		strcat(cadena, data[i]);
		strcat(cadena, ",");
	}

	cadena[strlen(cadena) - 1] = '\n';
	printf("cadena: %s\n", cadena);
	int ret = write(fd, cadena, strlen(cadena));


}


//closes csv file
void CSV_close(int fd){
	if(-1 == close(fd)) perror("csv close");
}


/* reverse:  reverse string s in place */
void reverse(char s[])
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

// reverses a string 'str' of length 'len'
void _reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

// Converts a given integer x to string str[].  d is the number
// of digits required in output. If d is more than the number
// of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x%10) + '0';
		x = x/10;
	}

	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
		str[i++] = '0';

	_reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating point number to string.
char* ftoa(float n, char *res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// convert integer part to string
	int i = intToStr(ipart, res, 0);

	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot

		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);

		intToStr((int)fpart, res + i + 1, afterpoint);
	}

	return res;
}

/* itoa:  convert n to characters in s */
char* itoa(int n, char* s)
{
	int i, sign;

	if ((sign = n) < 0)  /* record sign */
		n = -n;          /* make n positive */
	i = 0;
	do {       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);

}

/*
   Double to ASCII Conversion without sprintf.
   Roughly equivalent to: sprintf(s, "%.14g", n);
*/

#include <string.h>
// For printf
#include <stdio.h>

static double PRECISION = 0.00000000000001;
static int MAX_NUMBER_STRING_SIZE = 32;

/**
 * Double to ASCII
 */
char * dtoa(char *s, double n) {
	// handle special cases
	if (isnan(n)) {
		strcpy(s, "nan");
	} else if (isinf(n)) {
		strcpy(s, "inf");
	} else if (n == 0.0) {
		strcpy(s, "0");
	} else {
		int digit, m, m1;
		char *c = s;
		int neg = (n < 0);
		if (neg)
			n = -n;
		// calculate magnitude
		m = log10(n);
		int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
		if (neg)
			*(c++) = '-';
		// set up for scientific notation
		if (useExp) {
			if (m < 0)
				m -= 1.0;
			n = n / pow(10.0, m);
			m1 = m;
			m = 0;
		}
		if (m < 1.0) {
			m = 0;
		}
		// convert the number
		while (n > PRECISION || m >= 0) {
			double weight = pow(10.0, m);
			if (weight > 0 && !isinf(weight)) {
				digit = floor(n / weight);
				n -= (digit * weight);
				*(c++) = '0' + digit;
			}
			if (m == 0 && n > 0)
				*(c++) = '.';
			m--;
		}
		if (useExp) {
			// convert the exponent
			int i, j;
			*(c++) = 'e';
			if (m1 > 0) {
				*(c++) = '+';
			} else {
				*(c++) = '-';
				m1 = -m1;
			}
			m = 0;
			while (m1 > 0) {
				*(c++) = '0' + m1 % 10;
				m1 /= 10;
				m++;
			}
			c -= m;
			for (i = 0, j = m-1; i<j; i++, j--) {
				// swap without temporary
				c[i] ^= c[j];
				c[j] ^= c[i];
				c[i] ^= c[j];
			}
			c += m;
		}
		*(c) = '\0';
	}
	return s;
}


float promediate(float * values, int nvalues){

	float acum = 0;
	int i;
	int dividend = 0;

	for(i = 0; i < nvalues ; i++){
		if (values[i] != -1){
			acum += values[i];
			dividend++;
		}
		printf("value at %d is %.2f!\n", i, values[i]);
	}
	return acum/dividend;
}
