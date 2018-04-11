//
// Created by guidola on 1/14/16.
//
#include <stdio.h>
#include "orderedBuffer.h"


OrderedBuffer OB_create(uint32_t size){

    OrderedBuffer ob;
    ob.data = (OB_Node*) calloc(size, sizeof(OB_Node));
    ob.index = 0;
    ob.size = size;
    SEM_constructor(&(ob.sem));
    SEM_init(&(ob.sem), 0);
    ob.blocked = 0;
    ob.expected_tsn = 0;
    return ob;
}

int OB_destroy(OrderedBuffer* ob){

    free(ob->data);
    ob->index = 0;
    ob->size = 0;
    ob->blocked = 0;
    ob->expected_tsn = 0;
    SEM_destructor(&(ob->sem));
    return 1;
}

int OB_insert(OrderedBuffer* ob, uint8_t data[DATA_SIZE], uint32_t tsn){

    int i;

    if(ob->index == 0){
        ob->data[0].tsn = tsn;
        memcpy(ob->data[0].data, data, DATA_SIZE);
        (ob->index)++;
        ob->higher_tsn = ob->data[ob->index - 1].tsn;
        //check if 0 cool then notify to pipe
        if(tsn == ob->expected_tsn && ob->blocked) SEM_signal(&(ob->sem));
        return 1;
    }

    if(ob->index == ob->size) return 0;

    for(i = ob->index - 1 ; i >= 0 ; i--){
        if(ob->data[i].tsn < tsn){
            memmove(&(ob->data[i + 2]), &(ob->data[i + 1]), ob->index - 1 - i);
            ob->data[i + 1].tsn = tsn;
            memcpy(ob->data[i + 1].data, data, DATA_SIZE);
            (ob->index)++;
            ob->higher_tsn = ob->data[ob->index - 1].tsn;
            //check if 0 cool then notify to pipe
            if(ob->data[0].tsn == ob->expected_tsn && ob->blocked) SEM_signal(&(ob->sem));
            return 1;
        }
    }
    //
    return -1;
}

int OB_pull(OrderedBuffer* ob, uint32_t expected_tsn, uint8_t data[DATA_SIZE]){


    if(ob->data[0].tsn == expected_tsn){
        memcpy(data, ob->data[0].data, DATA_SIZE);
        if(ob->index > 1) memmove(ob->data, &(ob->data[1]), ob->index - 1);
        (ob->index)--;
        return 1;
    }


    ob->blocked = 1;
    SEM_wait(&(ob->sem));
    ob->blocked = 0;

    memcpy(data, ob->data[0].data, DATA_SIZE);
    if(ob->index > 1) memmove(ob->data, &(ob->data[1]), ob->index - 1);
    (ob->index)--;

    return 1;
}

int OB_isEmpty(OrderedBuffer ob){

    return ob.index == 0;
}


uint32_t OB_getHigherTsn(OrderedBuffer ob){

    return ob.data[ob.index - 1].tsn;
}

void __debugOutput(OrderedBuffer ob){

    int i;

    printf("TSN\n");
    for (i = 0; i < ob.index; i++) {
        printf("%u\n", ob.data->tsn);
    }

}