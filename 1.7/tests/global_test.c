#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "uthread.h"


void *f1(){
    for (int i = 0; i < 6; ++i){
        printf("[thread 1]_%d\n", i);
        sleep(1);
        uthread_yield();
    }
    return NULL;
}

void *f2(){
    for (int i = 0; i < 4; ++i){
        printf("[thread 2]_%d\n", i);
        sleep(1);
        uthread_yield();
    }
    return NULL;
}


void *f3(){
    for (int i = 0; i < 3; ++i){
        printf("[thread 3]_%d\n", i);
        sleep(1);
        uthread_yield();
    }
    return NULL;
}

int main(void){
    int err;
    uthread_t ut1, ut2, ut3;
   
    err = uthread_create(&ut1, f1, NULL);
    if (err){
        printf("uthread_create() failed: %s\n", strerror(errno));
        return 1;
    }
    err = uthread_create(&ut2, f2, NULL);
    if (err){
        printf("uthread_create() failed: %s\n", strerror(errno));
        return 1;
    }   

    for (int i = 0; i < 2; ++i){
        printf("[main]_%d\n", i);
        sleep(1);
        uthread_yield();
    }

    uthread_join(ut1, NULL);

    err = uthread_create(&ut3, f3, NULL);
    uthread_join(ut3, NULL);
    return 0;
}
