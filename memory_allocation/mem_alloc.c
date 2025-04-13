#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAXHEAP 64000
#define MAXCHUNKS 1024

typedef struct {
    void *start;
    size_t size;
}Chunk;

size_t chunk_count = 0;
char heap[MAXHEAP] = { 0 };
size_t curr_heap_size = 0;
Chunk chunks[MAXCHUNKS];

Chunk *allocate(size_t size) {
    if (size == 0)
        return NULL;
    assert(chunk_count < MAXCHUNKS);

    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Mmap failed\n");
        return NULL;
    }
    chunks[chunk_count].start = mem;
    chunks[chunk_count].size = size;
    chunk_count++;
    return &chunks[chunk_count - 1];
}

void deallocate(void *start) {
    for (size_t i = 0; i < chunk_count; ++i) {
        if (chunks[i].start == start && chunks[i].size > 0) {
            munmap(chunks[i].start, chunks[i].size);
            chunks[i].size = 0;
            return;
        }
    }
    perror("Invalid deallocate attempt\n");
}

int main(int argc, char **argv) {

    return 0;
}

