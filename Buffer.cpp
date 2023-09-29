//
// Created by os on 5/17/23.
//

#include "../h/Buffer.h"


int Buffer::append(char c) {
    buffer[tail] = c;
    tail = (tail + 1) % BufferSize;
    numOfElems++;
    return 0;
}

char Buffer::take() {
    char c = buffer[head];
    head = (head + 1) % BufferSize;
    numOfElems--;
    return c;
}

Buffer *Buffer::createBuffer() {
    size_t numOfBlocks = (sizeof(Buffer) + MemoryAllocator::sizeOfHeader + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    Buffer* buffer = (Buffer*)MemoryAllocator::MemAllocObj().k_mem_alloc(numOfBlocks);
    buffer->head = 0;
    buffer->tail = 0;
    buffer->numOfElems = 0;
    return buffer;
}


