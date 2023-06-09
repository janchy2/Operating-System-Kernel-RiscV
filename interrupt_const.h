//
// Created by os on 4/8/23.
//

#ifndef INTERRUPT_CONST_H
#define INTERRUPT_CONST_H

const static uint64 TIMER = 0x8000000000000001;
const static uint64 HARDWARE = 0x8000000000000009;
const static uint64 ECALL_USR = 8;
const static uint64 ECALL_SYS = 9;

const static uint64 MEM_ALLOC = 0x01;
const static uint64 MEM_FREE = 0x02;
const static uint64 THREAD_CREATE = 0x11;
const static uint64 THREAD_EXIT = 0x12;
const static uint64 THREAD_DISPATCH = 0x13;
const static uint64 THREAD_JOIN = 0x14;
const static uint64 SEM_OPEN = 0x21;
const static uint64 SEM_CLOSE = 0x22;
const static uint64 SEM_WAIT = 0x23;
const static uint64 SEM_SIGNAL = 0x24;
const static uint64 TIME_SLEEP = 0x31;
const static uint64 GETC = 0x41;
const static uint64 PUTC = 0x42;


#endif //INTERRUPT_CONST_H
