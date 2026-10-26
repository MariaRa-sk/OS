#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "mythread.h"


/*
Тест: 
Создание потока в потоке + join

Ожидаемый результат: 
Корректно отработают все потоки
*/



void *second_thread(void *arg) {
    printf("second: tid = %d, pid = %d, ppid = %d\n", gettid(), getpid(), getppid());
    return NULL;
}

void *first_thread(void *arg) {
    mythread_t second;
    int err;
    printf("first: tid = %d, pid = %d, ppid = %d\n", gettid(), getpid(), getppid());

    err = mythread_create(&second, second_thread, NULL);
    if (err) {
        printf("mythread_create() failed: %s\n", strerror(errno));
        return NULL;
    }

    err = mythread_join(second, NULL);
    if (err) {
        printf("mythread_join() failed: %s\n", strerror(errno));
        return NULL;
    }

    return NULL;
}

int main(void) {
    mythread_t first;
    int err;

    err = mythread_create(&first, first_thread, NULL);
    if (err) {
        printf("mythread_create() failed: %s\n", strerror(errno));
        return 1;
    }

    err = mythread_join(first, NULL);
    if (err) {
        printf("mythread_join() failed: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
