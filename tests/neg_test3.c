#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "mythread.h"


/*
Тест: 
Передача NULL в качестве первого аргумента mythread_join (m)

Ожидаемый результат:
mythread_join() завершится с ошибкой: Invalid argument
*/


void *thread_func(void *arg){
    printf("thread: pid = %d, tid = %d\n", getpid(), gettid());
    return NULL; 
}

int main(void){
    mythread_t m;
    int err;

    err = mythread_create(&m, thread_func, NULL);
    if (err){
        printf("mythread_create() failed: %s\n", strerror(errno));
        return 1;
    }


    err = mythread_join(NULL, NULL);
    if (err){
        printf("mythread_join() failed: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}
