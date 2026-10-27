#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "mythread.h"

/*
Тест: 
Создание потока c бесконечным циклом + join без возврата результат

Ожидаемый результат: 
Процесс не завершится до принудительной остановки
*/


void *thread_func(void *arg){
    while(1){
        printf("hello world\n");
    }   
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

    err = mythread_join(m, NULL);
    if (err){
        printf("mythread_join() failed: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}
