#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include "uthread.h"

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

void *counter_func(void *arg) {
    int *val = (int *)arg;
    return (void *)(intptr_t)((*val) * 2);
}

void *printer_func(void *arg) {
    int id = (int)(intptr_t)arg;
    for (int i = 0; i < 2; ++i){
        printf("[thread_%d] шаг %d\n", id, i);
        uthread_yield();
    }
    return NULL;
}

void pos_test1(void){
    uthread_t t;
    void *ret_val;
    int val = 100;
    printf("TEST 1: передача аргумента и возврат значения ");
    if (uthread_create(&t, counter_func, &val) != 0 || 
        uthread_join(t, &ret_val) != 0 || 
        (intptr_t)ret_val != 200){
        printf(RED "[FAILED]\n" RESET);
    }
    else {
        printf(GREEN "[PASSED]\n" RESET);
    }
}

void pos_test2(void) {
    uthread_t t1, t2;
    printf("TEST 2: два потока с передачей управления друг другу\n");
    if (uthread_create(&t1, printer_func, (void*)(intptr_t)1) != 0 || 
        uthread_create(&t2, printer_func, (void*)(intptr_t)2) != 0 ||
        uthread_join(t1, NULL) != 0 || uthread_join(t2, NULL) != 0){
        printf(RED "[FAILED]\n" RESET);
    }
    else{
        printf(GREEN "[PASSED]\n" RESET);
    }
}

void pos_test3(void) {
    uthread_t t;
    int val = 5;
    printf("TEST 3: uthread_join() с ret_val == NULL ");
    if (uthread_create(&t, counter_func, &val) != 0 || uthread_join(t, NULL) != 0){
        printf(RED "[FAILED]\n" RESET);
    }
    else{
        printf(GREEN "[PASSED]\n" RESET);
    }
}

void neg_test1(void) {
    uthread_t t;
    printf("TEST 4: uthread_create() с start_routine == NULL ");
    if (uthread_create(&t, NULL, NULL) != -1 || errno != EINVAL){
        printf(RED "[FAILED]\n" RESET);
    } 
    else{
        printf(GREEN "[PASSED]\n" RESET);
    }
}

void neg_test2(void){
    printf("TEST 5: uthread_join() с NULL вместо потока ");
    if (uthread_join(NULL, NULL) != -1 || errno != EINVAL){
        printf(RED "[FAILED]\n" RESET);
    } 
    else{
        printf(GREEN "[PASSED]\n" RESET);
    }
}

void neg_test3(void){
    uthread_t t;
    int val = 7;
    printf("TEST 6: Двойной uthread_join() ");
    if (uthread_create(&t, counter_func, &val) != 0){
        printf(RED "[FAILED] create\n" RESET);
        return;
    }
    if (uthread_join(t, NULL) != 0){
        printf(RED "[FAILED] first join\n" RESET);
        return;
    }
    
    if (uthread_join(t, NULL) != -1 || errno != EINVAL){
        printf(RED "[FAILED] second join\n" RESET);
    } else {
        printf(GREEN "[PASSED]\n" RESET);
    }
}

int main(void){
    pos_test1();
    pos_test2();
    pos_test3();
    neg_test1();
    neg_test2();
    neg_test3();
    return 0;
}