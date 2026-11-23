#include <stdio.h>
#include <errno.h>
#include "uthread.h"

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

void *thread_func(void *arg){
    return NULL; 
}

int main(void){
    uthread_t threads[MAX_THREADS];
    int created = 0;

    for (int i = 0; i < MAX_THREADS; ++i){
        if (uthread_create(&threads[i], thread_func, NULL) == 0){
            created++;
        } 
        else{
            break;
        }
    }

    //один слот для потока main
    printf("Ожидалось пользовательских потоков: %d\n", MAX_THREADS - 1);
    printf("Создано пользовательских потоков: %d\n", created);
    if (created == MAX_THREADS - 1 && errno == EAGAIN){
        printf(GREEN "[PASSED]\n" RESET);
    } 
    else{
        printf(RED "[FAILED] created = %d errno = %d\n" RESET, created, errno);
    }

    for (int i = 0; i < created; ++i){
        uthread_join(threads[i], NULL);
    }
    return 0;
}