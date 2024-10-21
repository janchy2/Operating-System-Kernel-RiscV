//
// Created by os on 4/9/23.
//
#include "../h/syscall_c.hpp"


inline uint64 readReturnValue() {

    uint64 result;
    __asm__ volatile("mv %[variable], a0" : [variable] "=r" (result)); //citanje povratne vrednosti
    return result;
}


void* mem_alloc(size_t size) {
    size = (size + MemoryAllocator::sizeOfHeader + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE;
    if(size <= 0) return nullptr;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(size)); //cuvanje argumenta u a1
    uint64 code = 0x01;
    //__asm__ volatile("csrs sip, 0x02"); //postavljen zahtev za softverskim prekidom
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    uint64 result;
    __asm__ volatile("mv %[variable], a0" : [variable] "=r" (result)); //citanje povratne vrednosti
    return (void*)result;
}

int mem_free(void* address) {
    if(!address) return -1;
    uint64 addr = (uint64) address;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(addr)); //cuvanje argumenta u a1
    uint64 code = 0x02;
    //__asm__ volatile("csrs sip, 0x02"); //postavljen zahtev za softverskim prekidom
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    return readReturnValue();
}


int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg) {
    if(!start_routine) return -1;

    void* stack_space = allocateStack();
    if(!stack_space) return -1;

    uint64 argument = (uint64)stack_space;
    __asm__ volatile("mv a4, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a4
    argument = (uint64)arg;
    __asm__ volatile("mv a3, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a3
    argument = uint64(start_routine);
    __asm__ volatile("mv a2, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a2
    argument = (uint64)handle;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a1

    uint64 code = 0x11;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    return readReturnValue();
}

int thread_exit () {
    uint64 code = 0x12;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    return readReturnValue();
}

void thread_dispatch () {
    uint64 code = 0x13;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");
}

void thread_join (thread_t handle) {

    uint64 argument = uint64(handle);
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a1

    uint64 code = 0x14;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");
}

int sem_open (sem_t* handle, unsigned init) {

    uint64 argument = uint64(init);
    __asm__ volatile("mv a2, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a2
    argument = (uint64)handle;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a1

    uint64 code = 0x21;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

   return readReturnValue();
}

int sem_close (sem_t handle) {

    uint64 argument = (uint64)handle;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a1

    uint64 code = 0x22;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    return readReturnValue();
}

int sem_wait (sem_t id) {

    uint64 argument = (uint64)id;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a1

    uint64 code = 0x23;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    return readReturnValue();
}

int sem_signal (sem_t id) {

    uint64 argument = (uint64)id;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a1

    uint64 code = 0x24;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    return readReturnValue();
}

int time_sleep(time_t time) {

    if(time <= 0) return -1; //nevalidno vreme uspavljivanja

    uint64 argument = (uint64)time;
    __asm__ volatile("mv a1, %[value]" : : [value] "r"(argument)); //cuvanje argumenta u a1

    uint64 code = 0x31;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    return 0;
}

char getc() {

    uint64 code = 0x41;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

    char result;
    __asm__ volatile("mv %[variable], a0" : [variable] "=r" (result)); //citanje povratne vrednosti
    return result;
}

void putc(char c) {

    __asm__ volatile("mv a1, %[value]" : : [value] "r"(c)); //cuvanje argumenta u a1

    uint64 code = 0x42;
    __asm__ volatile("mv a0, %[value]" : : [value] "r"(code)); //u a0 upisuje kod poziva

    __asm__ volatile("ecall");

}

void* allocateStack() {
    void* stack_space = mem_alloc(DEFAULT_STACK_SIZE); //alokacija steka
    return (char *)stack_space + DEFAULT_STACK_SIZE; //raste ka nizim adresama
}
