#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "mythread.h"


/*
Тест: 
Передача NULL в качестве первого аргумента mythread_create (mt)

Ожидаемый результат:
mythread_create() завершится с ошибкой: Invalid argument
Поток не создастся
*/


void *thread_func(void *arg){
    printf("thread: pid = %d, tid = %d\n", getpid(), gettid());
    return NULL; 
}

int main(void){
    mythread_t m = NULL;
    int err;

    err = mythread_create(NULL, thread_func, NULL);
    if (err){
        printf("mythread_create() failed: %s\n", strerror(errno));
        return 1;
    }


    err = mythread_join(m, NULL);
    if (err){
        printf("mythread_join() failed: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}
