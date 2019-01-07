# Merge Sort Demo with Work Stealing Algorithm

This repository provides merge sort demo with work stealing algorithm.

## How to Build

Use `make`.

## Programs

### merge_sort

simple merge sort using only main thread.

### split_merge_sort

merge sort which splits array for each worker thread at first 
and merges split arrays after each worker thread sorted its array.

