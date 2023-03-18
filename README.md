# CS149 Assignment 1 (WIP)

The solutions follow the assignments of Stanford CS149, https://gfxcourses.stanford.edu/cs149/fall21

If this violates any policy, please contact me directly.

## prog1

Summary:
* Parallelize a serial program using pthread. 
* Load balance all threads to reach better performance.

Question to be solved:
* Performance Speed Up is not linear to thread counts.
  * Performance usually drops at 7 and 21 threads
  * Given that my CPU is i7-12700H with 6 performance-cores and 8 efficient-cores, 20 threads in total, there should be some connections to it.

For i7-12700H, the 6 P-cores has two logical cores. The processor number of these logical cores are ranged from 0 to 11 in the `/proc/cpuinfo` file. (This can be inferred from the `core id` field)

We can inspect on which logical core our thread is running with `sched_getcpu()` (at least worked on my Ubuntu platform, there may have other ways to find this information). Here is an inspection of thread-core relationship when we have 6 threads:
```shell
Thread 2 time for execution is  [54.337]ms      running on core 0
Thread 5 time for execution is  [54.351]ms      running on core 4
Thread 4 time for execution is  [54.382]ms      running on core 8
Thread 0 time for execution is  [54.473]ms      running on core 6
Thread 3 time for execution is  [54.436]ms      running on core 2
Thread 1 time for execution is  [54.524]ms      running on core 10
```
Threads are scheduled on P-cores, each physical core is assigned exactly one thread. I guess this is because P-core has the greatest performance when only one thread is using it (assume my PC is idle, i.e. other application processes are light and have low priority), since hyper-threading technique does not duplicate every piece of a physical core but a limited modules like ALUs. 

If we increase it to 7 threads:
```shell
Thread 4 time for execution is  [46.828]ms      running on core 2
Thread 2 time for execution is  [47.062]ms      running on core 0
Thread 1 time for execution is  [47.148]ms      running on core 10
Thread 5 time for execution is  [47.469]ms      running on core 8
Thread 0 time for execution is  [47.719]ms      running on core 4
Thread 3 time for execution is  [47.954]ms      running on core 6
Thread 6 time for execution is  [56.772]ms      running on core 4
```
Notice that one physical core (P-core) is assigned to two threads. This may be the reason why there is a performance drop even when threads number increases (but not exceeds logical threads number).

* However, it is hard to explain why one logical core with two threads runs only 56ms while one thread runs for 47ms. I guess it relates to my codes in which I only record the logical core at the end of a thread, while this assumption is too strong. Thereby, I modify it to record core id and how many loop iterations it is used (in the parentheses after each core below), to provide a finer granularity.

For 6 threads (the perfect scheduled one):
```shell
Thread 4 time for execution is  [98.517]ms      running on core 10(200) 
Thread 3 time for execution is  [98.547]ms      running on core 4(200) 
Thread 1 time for execution is  [98.616]ms      running on core 6(200) 
Thread 0 time for execution is  [98.932]ms      running on core 8(200) 
Thread 2 time for execution is  [98.924]ms      running on core 0(200) 
Thread 5 time for execution is  [98.904]ms      running on core 2(200) 
```

For 6 threads (less perfect scheduled one, may be due to my PC turbulence):
```shell
Thread 2 time for execution is  [96.436]ms      running on core 4(200) 
Thread 3 time for execution is  [96.508]ms      running on core 10(200) 
Thread 1 time for execution is  [96.686]ms      running on core 2(200) 
Thread 4 time for execution is  [96.636]ms      running on core 0(156) 1(44) 
Thread 0 time for execution is  [97.471]ms      running on core 6(200) 
Thread 5 time for execution is  [103.765]ms     running on core 7(20) 8(122) 13(58) 
```

For 7 threads