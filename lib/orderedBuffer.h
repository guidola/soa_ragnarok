//
// Created by guidola on 1/14/16.
//

#ifndef MBTAP_ORDEREDBUFER_H
#define MBTAP_ORDEREDBUFER_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "semaphore.h"

#define DATA_SIZE 9000

typedef struct{

    uint32_t tsn;
    uint8_t data[DATA_SIZE];

}OB_Node;

typedef struct{

    OB_Node * data;
    uint32_t index;
    uint32_t size;
    semaphore sem;
    uint32_t expected_tsn;
    uint32_t higher_tsn;
    uint16_t blocked;

}OrderedBuffer;


OrderedBuffer OB_create(uint32_t size);
int OB_destroy(OrderedBuffer* ob);
int OB_pull(OrderedBuffer* ob, uint32_t expected_tsn, uint8_t data[DATA_SIZE]);
int OB_insert(OrderedBuffer* ob, uint8_t data[DATA_SIZE], uint32_t tsn);

int OB_isEmpty(OrderedBuffer ob);
uint32_t OB_getHigherTsn(OrderedBuffer ob);
void __debugOutput(OrderedBuffer ob);

#endif //MBTAP_ORDEREDBUFER_H
