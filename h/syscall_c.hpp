//
// Created by os on 4/9/23.
//
#ifndef SYSCALL_C_HPP
#define SYSCALL_C_HPP
#include "../lib/hw.h"
#include "../h/tcb.hpp"
#include "../h/K_Semaphore.h"


void* mem_alloc(size_t size);

int mem_free(void*);

typedef TCB* thread_t;

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_exit ();

void thread_dispatch ();

void thread_join (thread_t handle);

typedef K_Semaphore* sem_t;

int sem_open (sem_t* handle, unsigned init);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

int time_sleep(time_t time);

char getc();

void putc(char);

void* allocateStack();

#endif //SYSCALL_C_HPP
