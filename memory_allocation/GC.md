# Simple Mark-and-Sweep Garbage Collector in C

This project showcases a minimal implementation of a **Mark-and-Sweep Garbage Collector** written in C. 
It simulates a memory management system that can identify and clean up unreachable memory chunks, helping to prevent memory leaks in applications that manage dynamic memory.

## Concept Overview

Garbage collection is the process of automatically identifying and reclaiming memory that is no longer in use. This example implements a two-phase approach:

1. **Mark Phase**: Traverse the object graph starting from a set of *root pointers* and mark all reachable memory chunks as "alive."
2. **Sweep Phase**: Iterate through all allocated chunks and deallocate the ones that were not marked in the previous phase.

This is known as the **Mark-and-Sweep algorithm**.


## Code Structure

- `Chunk`: Structure to represent a memory chunk with metadata, including child pointers and mark status.
- `allocate(size_t size)`: Allocates a memory chunk using `mmap`.
- `deallocate(void *start)`: Frees a memory chunk.
- `gc_register_root(void *ptr)`: Registers a root pointer.
- `mark(void *ptr)`: Recursively marks reachable chunks.
- `sweep()`: Frees all unmarked chunks.
- `gc_collect()`: Performs a full garbage collection cycle.
