#define _GNU_SOURCE
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mythread.h"

#define THREADS_COUNT 5

/*
Тест: 
Создание 5 потоков с передачей аргументов + инкремент аргумента + возврат измененного аргумента

Ожидаемый результат: 
Корректно отработают все потоки, аргументы корректно изменятся
*/


void *thread_func(void *arg){
    int *a = (int *)arg;
    printf("thread: arg = %d, pid = %d, tid = %d\n", *a, getpid(), gettid());
    fflush(stdout); 
    (*a)++;
    return (void *)a;
}

int main(void){
    mythread_t threads[THREADS_COUNT];
    int args[THREADS_COUNT];
    int err;

    for (size_t i = 0; i < THREADS_COUNT; ++i) {
        args[i] = i + 1;
        err = mythread_create(&threads[i], thread_func, &args[i]);
        if (err) {
            printf("mythread_create() failed: %s\n", strerror(errno));
            return 1;
        }
    }

    for (size_t i = 0; i < THREADS_COUNT; ++i) {
        void *ret_val;
        err = mythread_join(threads[i], &ret_val);
        if (err) {
            printf("mythread_join() failed: %s\n", strerror(errno));
            return 1;
        }
        printf("joined %zu: ret_val = %d\n", i, *(int *)ret_val);
    }
    return 0;
}
