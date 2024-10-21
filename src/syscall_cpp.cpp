//
// Created by os on 5/23/23.
//
#include "../h/syscall_cpp.hpp"
#include "../h/riscv.hpp"

void* operator new (size_t size) {
    return mem_alloc(size);
}

void operator delete (void* pointer) {
    mem_free(pointer);
}

Thread::Thread(void (*body)(void *), void *arg) {

    TCB::shouldBeStarted = false;
    if(thread_create(&myHandle, body, arg)) myHandle = nullptr;
    TCB::shouldBeStarted = true;
    this->body = body;
    this->arg = arg;
}

void Thread::threadWrapper(void* pointer) {

    Thread* t = (Thread*)pointer;
    t->run();
}

Thread::Thread() {

    TCB::shouldBeStarted = false;
    if(thread_create(&myHandle, Thread::threadWrapper, (void*)this)) myHandle = nullptr;
    TCB::shouldBeStarted = true;
    this->body = nullptr;
    this->arg = nullptr;
}

int Thread::start() {

    if(!myHandle) return -1;
    Scheduler::SchedulerObj().put(myHandle);
    return 0;
}

void Thread::join() {

    thread_join(myHandle);
}

void Thread::dispatch() {

    thread_dispatch();
}

int Thread::sleep(time_t time) {

    return time_sleep(time);
}

Thread::~Thread() {

    myHandle->setPhase(TCB::FINISHED);
    myHandle->deleteStack();
    myHandle->deleteSemaphore();
    mem_free((void*)myHandle);
}

thread_t Thread::getHandle(Thread *thread) {
    return thread->myHandle;
}


Semaphore::Semaphore(unsigned int init) {

    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {

    sem_close(myHandle);
    mem_free((void*)myHandle);
}

int Semaphore::wait() {

    return sem_wait(myHandle);
}

int Semaphore::signal() {

    return sem_signal(myHandle);
}


PeriodicThread::PeriodicThread(time_t period): Thread() {

    this->period = period;
}

void PeriodicThread::terminate() {

    TCB* handle = getHandle(this);
    handle->setPhase(TCB::FINISHED);
    thread_dispatch();
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
