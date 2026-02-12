# Allocator Simulator

A terminal-based memory allocator simulator that visualizes heap layout changes step by step.
This program is designed to help understand how a typical implicit free-list allocator works
at a low level (headers, footers, alignment, splitting, and coalescing).

## Overview

This simulator mimics a simplified `malloc` / `free` implementation on a **64-bit system**.
Instead of actually allocating OS memory, it operates on a **simulated heap** and prints
the memory layout after every operation.

The focus is clarity over performance:  
you can see exactly how blocks are laid out in memory and how they change over time.

## Motivation

This project was built to deepen understanding of:

- How `malloc` works internally
- Why allocators use headers and footers
- How fragmentation occurs
- How coalescing restores large free blocks

## Key Assumptions

- 64-bit architecture
- 16-byte alignment
- Each block contains:
  - 8-byte header
  - payload
  - 8-byte footer
- Boundary-tag method (headers + footers)
- First-fit placement strategy

## Heap Model

- Initial heap size: **4 MB**
- Heap extension unit: **4 KB**
- Heap layout includes:
  - Alignment padding
  - Prologue block
  - Regular blocks (allocated / free)
  - Epilogue header

Initial logical layout:
padding | prologue | free block | epilogue

## Build

```bash
make
```

Outputs:
- `./allocator_sim`

Clean:

```bash
make clean
```

## Run

```bash
./allocator_sim
```

Example output:

```text
% ./allocator_sim 
This program demonstrates how a memory allocator works by visualizing the memory layout.
It uses a 4MB simulated heap, which is extended in 4KB increments.
The allocator assumes a 64-bit system with 16-byte alignment.
Each block contains an 8-byte header and an 8-byte footer.

Below is the initial state of the memory:

------------------------------------------------
0x10db7e020     8B padding
0x10db7e028     8B pro_H   [size=16 | alloc=1]
0x10db7e030     0B pro_P  
0x10db7e030     8B pro_F   [size=16 | alloc=1]
0x10db7e038     8B free_H  [size=4096 | alloc=0]
0x10db7e040  4080B free_P 
0x10db7f030     8B free_F  [size=4096 | alloc=0]
0x10db7f038     8B epi_H   [size=0 | alloc=1]
------------------------------------------------

Choose action (malloc/free/quit): malloc
How many bytes to malloc? 37
allocated index 0
------------------------------------------------
0x10db7e020     8B padding
0x10db7e028     8B pro_H   [size=16 | alloc=1]
0x10db7e030     0B pro_P  
0x10db7e030     8B pro_F   [size=16 | alloc=1]
0x10db7e038     8B alloc_H [size=64 | alloc=1]
0x10db7e040    48B alloc_P
0x10db7e070     8B alloc_F [size=64 | alloc=1]
0x10db7e078     8B free_H  [size=4032 | alloc=0]
0x10db7e080  4016B free_P 
0x10db7f030     8B free_F  [size=4032 | alloc=0]
0x10db7f038     8B epi_H   [size=0 | alloc=1]
------------------------------------------------

Choose action (malloc/free/quit): malloc
How many bytes to malloc? 992 
allocated index 1
------------------------------------------------
0x10db7e020     8B padding
0x10db7e028     8B pro_H   [size=16 | alloc=1]
0x10db7e030     0B pro_P  
0x10db7e030     8B pro_F   [size=16 | alloc=1]
0x10db7e038     8B alloc_H [size=64 | alloc=1]
0x10db7e040    48B alloc_P
0x10db7e070     8B alloc_F [size=64 | alloc=1]
0x10db7e078     8B alloc_H [size=1008 | alloc=1]
0x10db7e080   992B alloc_P
0x10db7e460     8B alloc_F [size=1008 | alloc=1]
0x10db7e468     8B free_H  [size=3024 | alloc=0]
0x10db7e470  3008B free_P 
0x10db7f030     8B free_F  [size=3024 | alloc=0]
0x10db7f038     8B epi_H   [size=0 | alloc=1]
------------------------------------------------

Choose action (malloc/free/quit): malloc
How many bytes to malloc? 5000
allocated index 2
------------------------------------------------
0x10db7e020     8B padding
0x10db7e028     8B pro_H   [size=16 | alloc=1]
0x10db7e030     0B pro_P  
0x10db7e030     8B pro_F   [size=16 | alloc=1]
0x10db7e038     8B alloc_H [size=64 | alloc=1]
0x10db7e040    48B alloc_P
0x10db7e070     8B alloc_F [size=64 | alloc=1]
0x10db7e078     8B alloc_H [size=1008 | alloc=1]
0x10db7e080   992B alloc_P
0x10db7e460     8B alloc_F [size=1008 | alloc=1]
0x10db7e468     8B alloc_H [size=5024 | alloc=1]
0x10db7e470  5008B alloc_P
0x10db7f800     8B alloc_F [size=5024 | alloc=1]
0x10db7f808     8B free_H  [size=2096 | alloc=0]
0x10db7f810  2080B free_P 
0x10db80030     8B free_F  [size=2096 | alloc=0]
0x10db80038     8B epi_H   [size=0 | alloc=1]
------------------------------------------------

Choose action (malloc/free/quit): free
allocated blocks:
  [0] 0x10db7e040
  [1] 0x10db7e080
  [2] 0x10db7e470
Choose index to free: 1
------------------------------------------------
0x10db7e020     8B padding
0x10db7e028     8B pro_H   [size=16 | alloc=1]
0x10db7e030     0B pro_P  
0x10db7e030     8B pro_F   [size=16 | alloc=1]
0x10db7e038     8B alloc_H [size=64 | alloc=1]
0x10db7e040    48B alloc_P
0x10db7e070     8B alloc_F [size=64 | alloc=1]
0x10db7e078     8B free_H  [size=1008 | alloc=0]
0x10db7e080   992B free_P 
0x10db7e460     8B free_F  [size=1008 | alloc=0]
0x10db7e468     8B alloc_H [size=5024 | alloc=1]
0x10db7e470  5008B alloc_P
0x10db7f800     8B alloc_F [size=5024 | alloc=1]
0x10db7f808     8B free_H  [size=2096 | alloc=0]
0x10db7f810  2080B free_P 
0x10db80030     8B free_F  [size=2096 | alloc=0]
0x10db80038     8B epi_H   [size=0 | alloc=1]
------------------------------------------------

Choose action (malloc/free/quit): free
allocated blocks:
  [0] 0x10db7e040
  [1] 0x10db7e470
Choose index to free: 1
------------------------------------------------
0x10db7e020     8B padding
0x10db7e028     8B pro_H   [size=16 | alloc=1]
0x10db7e030     0B pro_P  
0x10db7e030     8B pro_F   [size=16 | alloc=1]
0x10db7e038     8B alloc_H [size=64 | alloc=1]
0x10db7e040    48B alloc_P
0x10db7e070     8B alloc_F [size=64 | alloc=1]
0x10db7e078     8B free_H  [size=8128 | alloc=0]
0x10db7e080  8112B free_P 
0x10db80030     8B free_F  [size=8128 | alloc=0]
0x10db80038     8B epi_H   [size=0 | alloc=1]
------------------------------------------------

Choose action (malloc/free/quit): free
allocated blocks:
  [0] 0x10db7e040
Choose index to free: 0
------------------------------------------------
0x10db7e020     8B padding
0x10db7e028     8B pro_H   [size=16 | alloc=1]
0x10db7e030     0B pro_P  
0x10db7e030     8B pro_F   [size=16 | alloc=1]
0x10db7e038     8B free_H  [size=8192 | alloc=0]
0x10db7e040  8176B free_P 
0x10db80030     8B free_F  [size=8192 | alloc=0]
0x10db80038     8B epi_H   [size=0 | alloc=1]
------------------------------------------------

Choose action (malloc/free/quit): quit
```