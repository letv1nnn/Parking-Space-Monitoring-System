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

Chunk chunks[MAXCHUNKS];

Chunk *allocate(size_t size) {
    if (size == 0)
        return NULL;

    for (size_t i = 0; i < MAXCHUNKS; ++i) {
        if (chunks[i].size == 0) {
            void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (mem == MAP_FAILED) {
                perror("mmap failed");
                return NULL;
            }
            chunks[i].start = mem;
            chunks[i].size = size;
            return &chunks[i];
        }
    }
    fprintf(stderr, "No free chunks available\n");
    return NULL;
}

void deallocate(void *start) {
    for (size_t i = 0; i < MAXCHUNKS; ++i) {
        if (chunks[i].start == start && chunks[i].size > 0) {
            munmap(chunks[i].start, chunks[i].size);
            chunks[i].size = 0;
            return;
        }
    }
    perror("Invalid deallocate attempt");
}

int main(int argc, char **argv) {
    Chunk *ck = allocate(sizeof(int));
    int *num = (int *)ck->start;
    *num = 42;
    printf("%d\n", *num);
    deallocate(num);

    return 0;
}

