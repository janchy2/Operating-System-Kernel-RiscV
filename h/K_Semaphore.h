//
// Created by os on 4/23/23.
//

#ifndef K_SEMAPHORE_H
#define K_SEMAPHORE_H
#include "../h/tcb.hpp"

class TCB;

class K_Semaphore {
public:
    static void* allocateSemaphore();
    static int createSemaphore(K_Semaphore** handle, unsigned val);
    static int closeSemaphore(K_Semaphore* handle);
    int wait();
    int signal();
    int getValue() {return value;}
private:
    void block();
    void unblock();
    int value;
    int numOfBlocked;
    TCB* firstBlocked;
    TCB* lastBlocked;
    bool closed;
};


#endif //K_SEMAPHORE_H
