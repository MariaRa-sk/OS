#ifndef UTHREAD_H
#define UTHREAD_H

#define MAX_THREADS 100

typedef struct uthread uthread;
typedef struct uthread* uthread_t;

int uthread_create(uthread_t* thread, void *(*start_routine)(void *arg), void *arg);
int uthread_join(uthread_t ut, void **ret_val);
void uthread_yield(void);

#endif