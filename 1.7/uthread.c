#include <stdlib.h>
#include <errno.h>
#include <ucontext.h>
#include <stdio.h>
#include "uthread.h"

#define STACK_SIZE 64 * 1024

struct uthread{
    ucontext_t ctx;
    void *arg;
    void *(*start_routine)(void *);
    void *ret_val;
    char finished;
    void *stack;
};

uthread_t ut_arr[MAX_THREADS];
int ut_count = 0; 
int ut_alive = 0;
int ut_cur = 0;


static void scheduler(void){
    if (ut_count <= 1){
        return;
    }
    ucontext_t *cur_ctx, *next_ctx;
    int ut_next, cycle_start;

    cur_ctx = &ut_arr[ut_cur]->ctx;
    ut_next = (ut_cur + 1) % ut_count;
    cycle_start = ut_next;

    while(1){
        if (ut_arr[ut_next] && ut_arr[ut_next]->finished == 0){
            next_ctx = &ut_arr[ut_next]->ctx;
            ut_cur = ut_next;
            swapcontext(cur_ctx, next_ctx);
            return;
        }
        else{
            ut_next = (ut_next + 1) % ut_count;
            if (ut_next == cycle_start){
                return;
            }
        }
    }
}

void uthread_yield(void){
    scheduler();
}

static void startup(void){
    uthread_t ut = ut_arr[ut_cur];
    void *arg = ut->arg;
    ut->ret_val = ut->start_routine(arg);
    ut_arr[ut_cur]->finished = 1;
    scheduler();
}

static uthread_t uthread_main_init(void){
    int err;
    if (ut_count >= MAX_THREADS){
        errno = EAGAIN;
        return NULL;
    }
    uthread_t main_ut = malloc(sizeof(uthread));
    if (!main_ut){
        return NULL;
    }
    err = getcontext(&main_ut->ctx);
    if (err){
        free(main_ut);
        return NULL;
    }
    main_ut->finished = 0;
    ut_arr[ut_count] = main_ut;
    ut_count++;
    ut_alive++;
    return main_ut;
}

int uthread_create(uthread_t* thread, void *(*start_routine)(void *arg), void *arg){
    int err;
    if (start_routine == NULL || thread == NULL){
        errno = EINVAL;
        return -1;
    }

    //делаем main потоком
    if (ut_alive == 0){
        if (uthread_main_init() == NULL){
            return -1; 
        }
    }

    if (ut_alive >= MAX_THREADS){
        errno = EAGAIN;
        return -1;
    }

    *thread = malloc(sizeof(uthread));
    if (*thread == NULL){
        return -1;
    }

    (*thread)->stack = malloc(STACK_SIZE); 
    if (!(*thread)->stack){
        free(*thread);
        *thread = NULL;
        return -1;
    }

    err = getcontext(&(*thread)->ctx);
    if (err){
        free((*thread)->stack);
        free(*thread);
        *thread = NULL;
        return -1;
    }

    (*thread)->ctx.uc_stack.ss_sp = (*thread)->stack;
    (*thread)->ctx.uc_stack.ss_size = STACK_SIZE;
    (*thread)->ctx.uc_link = NULL;
    (*thread)->arg = arg;
    (*thread)->finished = 0;
    (*thread)->start_routine = start_routine;
   
    //переиспользование массива потоков
    int reused = -1;
    for (int i = 0; i < ut_count; ++i){
        if (ut_arr[i] == NULL){
            reused = i;
            break;
        }
    }
    
    if (reused != -1){
        ut_arr[reused] = *thread;
    }
    else{
        ut_arr[ut_count] = *thread;
        ut_count++;
    }
    ut_alive++;
    makecontext(&(*thread)->ctx, startup, 0);
    return 0;
}

int uthread_join(uthread_t ut, void **ret_val){
    if (!ut){
        errno = EINVAL;
        return -1;
    }

    int join_thread = -1;
    for (int i = 0; i < ut_count; ++i){
        if (ut_arr[i] == ut){
            join_thread = i;
            break;
        }
    }

    //поток не из массива, либо уже делали join, либо main поток
    if (join_thread == -1 || join_thread == 0){
        errno = EINVAL;
        return -1;
    }

    while(ut->finished != 1){
        scheduler();
    }
    if (ret_val){
        *ret_val = ut->ret_val;
    }

    ut_arr[join_thread] = NULL;
    ut_alive--;
    free(ut->stack);
    free(ut);
   
    return 0;
}
