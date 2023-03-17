# CS149 Assignment 1 (WIP)

The solutions follow the assignments of Stanford CS149, https://gfxcourses.stanford.edu/cs149/fall21

If this violates any policy, please contact me directly.

## prog1

Summary:
* Parallelize a serial program using pthread. 
* Load balance all threads to reach better performance.

Question to be solved:
* Performance Speed up is not linear to thread counts.
  * Performance usually drops at 7 and 21 threads
  * Given that my CPU is i7-12700H with 6 performance-cores and 8 efficient-cores, 20 threads in total, there should be some connections to it.
