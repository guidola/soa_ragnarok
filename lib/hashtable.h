#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef void* Element;
typedef uint32_t Index;


typedef struct n{
	Index index;
	Element value;
	struct n* next;
}Node_hash;

typedef Node_hash* Hashtable[100];

//*** CONST ***//


//*** PROTO ***//

int HASH_create(Hashtable);
int HASH_insert(Hashtable, Index, Element);
Element HASH_get(Hashtable, Index);
int HASH_delete(Hashtable, Index);
int HASH_destroy(Hashtable);


#endif