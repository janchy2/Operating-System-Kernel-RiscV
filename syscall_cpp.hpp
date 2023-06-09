//
// Created by os on 5/23/23.
//

#ifndef SYSCALL_CPP_HPP
#define SYSCALL_CPP_HPP
#include "syscall_c.hpp"

void* operator new (size_t size);
void operator delete (void* pointer);

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    void join();
    static void dispatch ();
    static int sleep (time_t time);
protected:
    Thread ();
    virtual void run () {}
    static thread_t getHandle(Thread* thread);
private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;
    static void threadWrapper(void* pointer);
    friend class TCB;
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate ();
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
private:
    time_t period;
};

class Console {
public:
    static char getc ();
    static void putc (char c);
};

#endif //SYSCALL_CPP_HPP
