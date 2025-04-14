#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAXCHUNKS 1024

// Main parts of GC work:
// 
// 1. MARK - going through each object starting with the root
// and markes all reachable objects as alive
//
// 2. SWEEP - removes allocated objects that was not been marked as alive
// 
// It's called  "Mark-and-Sweep" 

typedef struct {
    void *start;
    size_t size;
    int marked;
    void **ptrs;
    size_t ptr_count;
}Chunk;

Chunk chunks[MAXCHUNKS];
size_t chunk_count = 0;

// Extendded memory allocator and deallocator
Chunk *allocate(size_t size) {
    for (size_t i = 0; i < MAXCHUNKS; ++i) {
        if (chunks[i].size == 0) {
            void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            if (mem == MAP_FAILED) {
                perror("mmap failed");
                return 0;
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
    perror("Invalid deallocation attempt");
}

// GC should know where to look for alive objects.

#define MAX_ROOTS 128
void *roots[MAX_ROOTS];
size_t root_count = 0;

// Initialize a pointer as a root - a starting point for the GC to find live objects.
void gc_register_root(void *ptr) {
    if (root_count < MAX_ROOTS) {
        roots[root_count++] = ptr;
    }
}

// Marks all reachable objects.
void mark(void *ptr) {
    for (size_t i = 0; i < chunk_count; ++i) {       
        if (chunks[i].start == ptr && !chunks[i].marked && chunks[i].size > 0) {
            chunks[i].marked = 1;
            for (size_t j = 0; j < chunks[i].ptr_count; ++j) {
                mark(chunks[i].ptrs[j]);
            }
            return;
        }
    }
}

// Derallocate everything that is not marked.
void sweep() {
    for (size_t i = 0; i < chunk_count; ++i) {
        if (chunks[i].size > 0) {
            if (!chunks[i].marked) {
                munmap(chunks[i].start, chunks[i].size);
                chunks[i].size = 0;
                chunks[i].start = NULL;
            }
            else {
                chunks[i].marked = 0;
            }
        }
    }
}

// General gc collect function
void gc_collect() {
    for (size_t i = 0; i < root_count; ++i) {
        mark(roots[i]);
    }
    sweep();
}

int main() {
    Chunk *a = allocate(sizeof(void *) * 2);
    Chunk *b = allocate(sizeof(int));

    a->ptrs = malloc(sizeof(void *) * 1);
    a->ptr_count = 1;
    a->ptrs[0] = b->start;

    gc_register_root(a->start);
    gc_collect();
    a->ptrs[0] = NULL;
    gc_collect();

    return 0;
}

