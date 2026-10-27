#define _GNU_SOURCE
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <sched.h>           
#include <sys/syscall.h>    
#include <unistd.h>
#include <linux/futex.h> 
#include "mythread.h"

#define STACK_SIZE 8*1024*1024
#define GUARD_PAGE_SIZE 4096


int wrapper(void *arg){
    mythread *m = arg;
    m->ret_val = m->start_routine(m->arg);
    return 0;
}

int mythread_create(mythread_t *mt, void *(*start_routine)(void *arg), void *arg){
    void *stack;
    void *stackTop; 
    pid_t pid;
    int err;

    if (start_routine == NULL || mt == NULL){
        errno = EINVAL;
        return -1;
    }
  
    stack = mmap(NULL, STACK_SIZE + GUARD_PAGE_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0);
    if (stack == MAP_FAILED){
        return -1;
    }

    if (mprotect((char *)stack + GUARD_PAGE_SIZE, STACK_SIZE, PROT_READ|PROT_WRITE) != 0){
        err = errno;
        munmap(stack, STACK_SIZE + GUARD_PAGE_SIZE);
        errno = err;
        return -1;
    }

    stackTop = (char *)stack + GUARD_PAGE_SIZE + STACK_SIZE;

    *mt = (mythread *)((char *)stackTop - sizeof(mythread));
    (*mt)->start_routine = start_routine;
    (*mt)->arg = arg;
    (*mt)->stack = stack;

    pid = clone(wrapper, (void *)*mt, CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD \
            |CLONE_SYSVSEM|CLONE_CHILD_SETTID|CLONE_CHILD_CLEARTID|CLONE_PARENT_SETTID, *mt, \
            &(*mt)->tid, NULL, &(*mt)->tid);

    if (pid == -1){
        err = errno;
        munmap(stack, STACK_SIZE + GUARD_PAGE_SIZE);
        errno = err;
        return -1;
    }
    return 0;
}   

int mythread_join (mythread_t m, void **ret_val){
    if (m == NULL){
        errno = EINVAL;
        return -1;
    }
    int expected = m->tid;
    if (expected == 0){
        if (ret_val != NULL) *ret_val = m->ret_val;
        if (munmap(m->stack, STACK_SIZE + GUARD_PAGE_SIZE) == -1) return -1;
        return 0;
    }
    while (m->tid != 0){
        long err;
        err = syscall(SYS_futex, &m->tid, FUTEX_WAIT, expected, NULL, NULL, 0);
        
        //m->child_pid != expected
        if (err == -1 && errno == EAGAIN){
            if (m->tid == 0){
                if (ret_val != NULL) *ret_val = m->ret_val;
                if (munmap(m->stack, STACK_SIZE + GUARD_PAGE_SIZE) == -1) return -1;
                return 0;
            }
            else {
                return -1;
            }
        }

        //операция была прервана сигналом
        else if (err == -1 && errno == EINTR){
            continue;
        }
        else if (err == -1){
            return -1;
        }
    }
    if (ret_val != NULL) *ret_val = m->ret_val;
    if (munmap(m->stack, STACK_SIZE + GUARD_PAGE_SIZE) == -1) return -1;
    return 0;
}
