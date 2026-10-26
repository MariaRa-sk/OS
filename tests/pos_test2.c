#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mythread.h"

/*
Тест: 
Создание потока + join с возвратом результата

Ожидаемый результат: 
Распечатается правильное возвращаемое значение
*/


void *thread_func(void *arg){
    return (void *)52; 
}

int main(void){
    mythread_t m;
    int err;
    void* ret_val;

    err = mythread_create(&m, thread_func, NULL);
    if (err){
        printf("mythread_create() failed: %s\n", strerror(errno));
        return 1;
    }
    
    err = mythread_join(m, &ret_val);
    if (err){
        printf("mythread_join() failed: %s\n", strerror(errno));
        return 1;
    }
    printf("ret_val = %ld\n", (intptr_t)ret_val);
    return 0;
}
