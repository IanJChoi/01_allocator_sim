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

## Build, Run, Clean

This project uses a simple `Makefile`.
```bash
make
./allocator_sim
make clean
