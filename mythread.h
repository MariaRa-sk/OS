#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <sys/types.h>

typedef struct mythread {
    pid_t tid;
    void *ret_val;
    void *(*start_routine)(void *);
    void *arg;
    void *stack;
} mythread;

typedef mythread *mythread_t;

int mythread_create(
    mythread_t *mt,
    void *(*start_routine)(void *),
    void *arg
);

int mythread_join(mythread_t m, void **ret_val);

#endif
