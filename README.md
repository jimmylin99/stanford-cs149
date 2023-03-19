# CS149 Assignment 1 (WIP)

The solutions follow the assignments of Stanford CS149, https://gfxcourses.stanford.edu/cs149/fall21

If this violates any policy, please contact me directly.

## prog1

Summary:
* Parallelize a serial program using pthread. 
* Load balance all threads to reach better performance.
  * the trick itself is trivial, but I did not come up with it at the first glance (more practice!)
  * I kind of got stuck at division-and-conquer, while for arbitrarily complicated pattern, split the workload into tiny pieces and disperse them (which may be more trivial) is better.
    * This idea is similar to the differential in calculus: for arbitrarily continuous function, the local behaviors are almost the same
* Automated the workflow with shell scripts and jupyter notebook (a nice attempt after not using them for a long time)
* Experiments to inspect on thread scheduling on heterogeneous architecture (interesting! there is more to discover yet)

Question to be resolved:
* Performance Speed Up is not linear to thread counts.
  * Performance usually drops at 7 and 21 threads
  * Given that my CPU is i7-12700H with 6 performance-cores and 8 efficient-cores, 20 threads in total, there should be some connections to it.

Experiment to attempt to answer it:

For i7-12700H, the 6 P-cores has two logical cores. The processor number of these logical cores are ranged from 0 to 11 in the `/proc/cpuinfo` file. (This can be inferred from the `core id` field)

We can inspect on which logical core our thread is running with `sched_getcpu()` (at least worked on my Ubuntu platform, there may have other ways to find this information). Here is an inspection of thread-core relationship when we have 6 threads:
```text
Thread 2 time for execution is  [54.337]ms      running on core 0
Thread 5 time for execution is  [54.351]ms      running on core 4
Thread 4 time for execution is  [54.382]ms      running on core 8
Thread 0 time for execution is  [54.473]ms      running on core 6
Thread 3 time for execution is  [54.436]ms      running on core 2
Thread 1 time for execution is  [54.524]ms      running on core 10
```
Threads are scheduled on P-cores, each physical core is assigned exactly one thread. I guess this is because P-core has the greatest performance when only one thread is using it (assume my PC is idle, i.e. other application processes are light and have low priority), since hyper-threading technique does not duplicate every piece of a physical core but a limited modules like ALUs. 

If we increase it to 7 threads:
```text
Thread 4 time for execution is  [46.828]ms      running on core 2
Thread 2 time for execution is  [47.062]ms      running on core 0
Thread 1 time for execution is  [47.148]ms      running on core 10
Thread 5 time for execution is  [47.469]ms      running on core 8
Thread 0 time for execution is  [47.719]ms      running on core 4
Thread 3 time for execution is  [47.954]ms      running on core 6
Thread 6 time for execution is  [56.772]ms      running on core 4
```
Notice that one physical core (P-core) is assigned to two threads. This may be the reason why there is a performance drop even when threads number increases (but not exceeds logical threads number).

* However, it is hard to explain why one logical core with two threads runs only 56ms while one thread runs for 47ms. I guess it relates to my codes in which I only record the logical core at the end of a thread, while this assumption is too strong. Thereby, I modify it to record core id and how many loop iterations it is used and time consumed (in the parentheses after each core below), to provide a finer granularity.

For 6 threads (the perfect scheduled one):
```text
Thread 1 time for execution is  [53.974]ms      running on core 6(200iters,54ms) 
Thread 2 time for execution is  [53.994]ms      running on core 0(200iters,54ms) 
Thread 3 time for execution is  [54.008]ms      running on core 2(200iters,54ms) 
Thread 5 time for execution is  [54.006]ms      running on core 10(200iters,54ms) 
Thread 4 time for execution is  [54.064]ms      running on core 8(200iters,54.1ms) 
Thread 0 time for execution is  [54.276]ms      running on core 4(200iters,54.3ms) 
```

For 6 threads (less perfect scheduled one, may be due to my PC turbulence):
```text
Thread 1 time for execution is  [54.000]ms      running on core 0(200iters,54ms) 
Thread 2 time for execution is  [53.990]ms      running on core 6(200iters,54ms) 
Thread 3 time for execution is  [54.036]ms      running on core 10(200iters,54ms) 
Thread 5 time for execution is  [54.865]ms      running on core 8(171iters,53.3ms) 17(29iters,1.51ms) 
Thread 0 time for execution is  [57.115]ms      running on core 4(200iters,57.1ms) 
Thread 4 time for execution is  [58.461]ms      running on core 2(126iters,40.9ms) 5(32iters,1.71ms) 17(42iters,15.8ms) 
```

For 7 threads (case 1)
```text
Thread 1 time for execution is  [46.286]ms      running on core 10(172iters,46.3ms) 
Thread 2 time for execution is  [46.300]ms      running on core 0(172iters,46.3ms) 
Thread 4 time for execution is  [46.419]ms      running on core 2(171iters,46.4ms) 
Thread 0 time for execution is  [46.816]ms      running on core 6(172iters,46.8ms) 
Thread 5 time for execution is  [48.878]ms      running on core 4(117iters,38.6ms) 7(27iters,1.42ms) 19(27iters,8.88ms) 
Thread 3 time for execution is  [54.914]ms      running on core 8(66iters,13.2ms) 9(34iters,2.13ms) 13(71iters,39.6ms) 
Thread 6 time for execution is  [58.365]ms      running on core 12(171iters,58.4ms) 
```

For 7 threads (case 2)
```text
Thread 2 time for execution is  [46.283]ms      running on core 2(172iters,46.3ms) 
Thread 3 time for execution is  [46.293]ms      running on core 4(171iters,46.3ms) 
Thread 0 time for execution is  [46.560]ms      running on core 6(172iters,46.6ms) 
Thread 1 time for execution is  [47.194]ms      running on core 10(172iters,47.2ms) 
Thread 6 time for execution is  [51.652]ms      running on core 0(101iters,32.4ms) 18(70iters,19.2ms) 
Thread 4 time for execution is  [54.605]ms      running on core 7(1iters,0.0116ms) 8(77iters,19.1ms) 16(93iters,35.5ms) 
Thread 5 time for execution is  [56.727]ms      running on core 4(57iters,9.27ms) 12(114iters,47.5ms) 
```

Though more data and precise experiments are needed, we can basically infer that the performance drop is largely due to the performance difference between P-core and E-core. Thread rescheduling may also affect the execution time a little.
