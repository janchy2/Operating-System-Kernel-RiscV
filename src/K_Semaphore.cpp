//
// Created by os on 4/23/23.
//

#include "../h/K_Semaphore.h"


void *K_Semaphore::allocateSemaphore() {
    size_t numOfBlocks = (sizeof(K_Semaphore) + MemoryAllocator::sizeOfHeader + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::MemAllocObj().k_mem_alloc(numOfBlocks);
}

int K_Semaphore::createSemaphore(K_Semaphore **handle, unsigned int val) {
    K_Semaphore* sem = (K_Semaphore*)allocateSemaphore();
    if(!sem) return -1; //nije uspela alokacija
    sem->value = (int)val;
    sem->firstBlocked = nullptr;
    sem->lastBlocked = nullptr;
    sem->numOfBlocked = 0;
    sem->closed = false;
    (*handle) = sem;
    return 0;
}

void K_Semaphore::block() {
    TCB* thread = TCB::running;
    if(!lastBlocked) firstBlocked = thread;
    else lastBlocked->setNextInSemaphore(thread);
    lastBlocked = thread;
    thread->setNextInSemaphore(nullptr);
    thread->setPhase(TCB::BLOCKED);
    numOfBlocked++;
    TCB::dispatch();
}

void K_Semaphore::unblock() {
    TCB* oldFirst = firstBlocked;
    firstBlocked = firstBlocked->getNextInSemaphore();
    oldFirst->setNextInSemaphore(nullptr);
    if(!firstBlocked) lastBlocked = nullptr;
    Scheduler::SchedulerObj().put(oldFirst);
    numOfBlocked--;
}

int K_Semaphore::wait() {
    if(closed) return -1;
    if(--value < 0) block();
    return 0;
}

int K_Semaphore::signal() {
    if(closed) return -1;
    if(++value <= 0) unblock();
    return 0;
}


int K_Semaphore::closeSemaphore(K_Semaphore *handle) {
    if(handle->closed) return -1;
    while(handle->numOfBlocked) {
        handle->unblock();
    }
    handle->closed = true;
    return 0;
}

