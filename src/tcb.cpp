#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

TCB *TCB::running = nullptr;

time_t TCB::timeSliceCounter = 0;

int TCB::id = 0;

TCB* TCB::firstInSleepQueue = nullptr;

bool TCB::shouldBeStarted = true;

int TCB::createTCB(TCB** handle, void (*start_routine)(void*), void* arg, void* stack_space, time_t timeSlice) {

    TCB* tcb = (TCB*)allocateTCB();
    if(!tcb) return -1; //nije uspela alokacija

    tcb->myId = id++; //dodela pid-a

    tcb->body = start_routine;

    tcb->argument = arg;

    tcb->context.sp = (uint64)stack_space; //dodela vrednosti korisnickom sp

    tcb->stack = (uint64*)stack_space;

    tcb->context.ra = (uint64)&wrapper; //pocinje izvrsavanje odatle

    K_Semaphore::createSemaphore(&(tcb->mySemaphore), 0); //pravi semafor koji se koristi za join

    tcb->timeSlice = timeSlice;

    *(handle) = tcb;

    tcb->phase = CREATED;

    if(shouldBeStarted) Scheduler::SchedulerObj().put(tcb);

    return 0;
}


void TCB::dispatch()
{
    TCB *old = running;
    if (old->isRunning()) Scheduler::SchedulerObj().put(old);
    running = Scheduler::SchedulerObj().get();
    running->setPhase(TCB::RUNNING);

    TCB::contextSwitch(&old->context, &running->context);
}


void *TCB::allocateTCB() {
    size_t numOfBlocks = (sizeof(TCB) + MemoryAllocator::sizeOfHeader + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::MemAllocObj().k_mem_alloc(numOfBlocks);
}

int TCB::exit() {

    running->setPhase(TCB::FINISHED);
    if(K_Semaphore::closeSemaphore(running->mySemaphore)) return -1; //pusta sve niti koje su pozvale join za tekucu
    dispatch();
    return 0;
}

void TCB::join(TCB *thread) {
    thread->mySemaphore->wait();
}

void TCB::sleep(time_t time) {
    TCB::running->setPhase(TCB::BLOCKED);

    TCB *curr = firstInSleepQueue, *prev = nullptr;
    time_t currTime = 0, prevTime = 0;
    for (; curr; prev = curr, curr = curr->nextInSleepQueue) {
        currTime += curr->differenceSleepTime;
        if (time < currTime) {
            if (curr == firstInSleepQueue) {
                firstInSleepQueue = TCB::running;
                TCB::running->differenceSleepTime = time; //prvi je u listi, razlika je celo vreme
            } else {
                prev->setNextInSleepQueue(TCB::running);
                TCB::running->differenceSleepTime = time - prevTime; //razlika u odnosu na prethodni
            }
            TCB::running->setNextInSleepQueue(curr);
            curr->differenceSleepTime -= TCB::running->differenceSleepTime;
            break;
        }
        prevTime = currTime;
    }
    if(!curr) {
        if(!prev) {
            firstInSleepQueue = TCB::running;
            TCB::running->differenceSleepTime = time; //prvi je u listi, razlika je celo vreme
        }
        else {
            prev->setNextInSleepQueue(TCB::running);
            TCB::running->differenceSleepTime = time - prev->differenceSleepTime; //razlika u odnosu na prethodni
        }
        TCB::running->setNextInSleepQueue(nullptr);
    }
}

void TCB::wakeUp() {

    while(firstInSleepQueue && firstInSleepQueue->differenceSleepTime == 0) {
        Scheduler::SchedulerObj().put(firstInSleepQueue);
        TCB* prev = firstInSleepQueue;
        firstInSleepQueue = firstInSleepQueue->nextInSleepQueue;
        prev->nextInSleepQueue = nullptr;
    }
}

void TCB::deleteSemaphore() {
    K_Semaphore::closeSemaphore(mySemaphore);
    MemoryAllocator::MemAllocObj().k_mem_free((void*)mySemaphore);
}

void TCB::wrapper() {
    Riscv::popSppSpie();
    running->body(running->argument);
    thread_exit();
}
