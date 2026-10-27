#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mythread.h"

/*
Тест: 
Создание потока c передачей аргумента в поточную функцию + join

Ожидаемый результат: 
В поточной функции распечатается переданный аргумент
*/


void *thread_func(void *arg){
    int a = (int)(intptr_t)arg;
    printf("arg = %d\n", a);
    return NULL; 
}

int main(void){
    mythread_t m;
    int err;
    int arg = 52;

    err = mythread_create(&m, thread_func, (void *)(intptr_t)arg);
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
