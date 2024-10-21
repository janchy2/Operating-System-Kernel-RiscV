//
// Created by os on 4/7/23.
//

#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H

#include "../lib/hw.h"

struct FreeMem {
    FreeMem* next;
    FreeMem* prev;
    size_t numOfBlocks;
};


class MemoryAllocator {
public:
    static FreeMem* freeMemHead;
    static const uint64 sizeOfHeader = sizeof(FreeMem);
    static MemoryAllocator& MemAllocObj();
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;
    void* k_mem_alloc(size_t numOfBlocks);
    int k_mem_free(void* address);
    void tryToMerge(FreeMem* p);
private:
    MemoryAllocator() {isMade = true;}
    ~MemoryAllocator() = default;
    static bool isMade;
};


#endif //MEMORYALLOCATOR_H
