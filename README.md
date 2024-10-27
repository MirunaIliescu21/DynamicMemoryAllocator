# Assignment Title: Dynamic Memory Allocator in C

## Description

This assignment focuses on building a memory allocator by implementing several core functionalities.

### Overview of Functionalities

The main functionalities are organized across different files, each handling specific commands.

#### 1. **Heap Initialization and Destruction** (`function.c`)

- **INIT_HEAP**: Initializes the heap by creating a data structure represented as a vector of doubly circularly linked lists.
This vector represents the free memory space available for operations. Each list position in the vector corresponds
to a block size, with the index + 1 indicating the block size.
  
- **DESTROY_HEAP**: Frees all allocated space, including both the vector of lists (handled by the `sfl_t` structure,
which manages free memory spaces) and the `allocated_list` (an additional circularly doubly linked list
created with `malloc` to track allocated memory).

#### 2. **Memory Allocation and Deallocation** (`function_malloc.c`)

- **MALLOC**: Searches for a block of a specified size within the `sfl_t` structure and transfers it to the `allocated_list`,
 marking it as allocated. If the available block is larger than requested, the block is split.
The required bytes are allocated, and the remainder is placed back into the appropriate list.
The vector’s design allows for efficient resizing-free reallocation.

- **FREE**: Searches for a block at a given address within `allocated_list`, removes it, and returns it to the `sfl_t` structure.

#### 3. **Reading and Writing Data** (`function_write_read.c`)

- **WRITE and READ**: These commands operate on the same principles, with one writing and the other reading.
  Reading is restricted to addresses where data has been previously written.
  Both commands handle segmentation faults by verifying the address ranges, ensuring safe access to the specified memory addresses.
  Data is read or written byte-by-byte into the `data` field of the `info_t` structure, which contains block information for each list.

### Key Learnings

From this assignment, I learned how to debug my code independently by working in a structured way and utilizing `#define` directives for debugging. 
Additionally, I gained a deeper understanding of memory allocation, writing, and reading processes.

