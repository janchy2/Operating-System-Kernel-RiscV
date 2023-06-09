#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "MemoryAllocator.h"
#include "../h/Scheduler.h"
#include "../h/K_Semaphore.h"

class K_Semaphore;

// Thread Control Block
class TCB
{
public:

    enum Phase{CREATED, READY, RUNNING, BLOCKED, FINISHED};

    static bool shouldBeStarted;
    void deleteStack() {MemoryAllocator::MemAllocObj().k_mem_free((void*)stack);}
    void deleteSemaphore();
    bool isFinished() {return phase == FINISHED;};
    bool isRunning() {return phase == RUNNING;}
    bool isReady() {return phase == READY;}
    void setPhase(Phase p) {phase = p;}

    static time_t timeSliceCounter;
    static TCB* firstInSleepQueue;
    uint64 getTimeSlice() const { return timeSlice; }

    TCB* getNextInScheduler() {return nextInScheduler;}
    void setNextInScheduler(TCB* next) {nextInScheduler = next;}
    TCB* getNextInSemaphore() {return nextInSemaphore;}
    void setNextInSemaphore(TCB* next) {nextInSemaphore = next;}
    TCB* getNextInSleepQueue() {return nextInSleepQueue;}
    void setNextInSleepQueue(TCB* next) {nextInSleepQueue = next;}

    int getId() {return myId;}

    static int createTCB(TCB** handle, void (*start_routine)(void*), void* arg, void* stack_space, time_t timeSlice = DEFAULT_TIME_SLICE);

    static TCB *running;

    static void dispatch();
    static int exit();
    static void join(TCB* thread);
    static void sleep(time_t time);
    static void wakeUp();

private:

    struct Context {

        uint64 ra;
        uint64 sp;
    };

    void(*body)(void*);
    void* argument;
    uint64 *stack;
    Context context;
    time_t timeSlice;
    time_t differenceSleepTime;
    Phase phase;
    static int id;
    int myId;
    K_Semaphore* mySemaphore;
    TCB* nextInScheduler;
    TCB* nextInSemaphore;
    TCB* nextInSleepQueue;

    static void wrapper();
    static void* allocateTCB();

    friend class Riscv;
    friend class K_Semaphore;

    static void contextSwitch(Context *oldContext, Context *runningContext);

};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
