#include <stdio.h>
#include <thread>
#include <cmath>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;


extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

//
// auxiliary functions START
int fact(int n) {
    if (n <= 1)
        return 1;
    return n * fact(n - 1);
}

int combination(int n, int r) {
    return fact(n) / (fact(r) * fact(n - r));
}
// auxiliary functions END

//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs * const args) {

    // TODO FOR CS149 STUDENTS: Implement the body of the worker
    // thread here. Each thread should make a call to mandelbrotSerial()
    // to compute a part of the output image.  For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.

    double startTime = CycleTimer::currentSeconds();

    // Not load balanced
//    int startRow = 0;
//    int n = args->numThreads;
//    int id = args->threadId;
//    int height = args->height;
//    int tot = 0;
//    {
//        int start = (n+1)/2;
//        //int start = exp2((n+1)/2-1);
//        tot = (1 + start) * start;
//        //tot = 2 * (2 * start - 1);
//        if (n % 2 == 1)
//            tot --;
//    }
//    float frac = 1.0 * args->height / tot;
//    int numRows = 0;
//    {
//        int piece = (n+1)/2;
//        //int piece = exp2((n+1)/2-1);
//        for (int i = 0; i <= id; i++) {
//            int numRowsForI = piece * frac;
//            if (numRowsForI == 0) numRowsForI = 1;
//            if (i < id)
//                startRow += numRowsForI;
//            else
//                numRows = numRowsForI;
//            if (i < (n-1)/2)
//                piece --;
//                //piece /= 2;
//            else if (!(i == (n-1)/2 && n % 2 == 0))
//                piece ++;
//                //piece *= 2;
//        }
//    }
//    // early exit due to too many threads
//    if (startRow >= height)
//        return;
//    // modify last necessary thread if precision issue exits
//    if (startRow + numRows > height)
//        numRows = height - startRow;
//    // modify last thread
//    if (id == n - 1)
//        numRows = height - startRow;
//
//    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
//                     args->width, args->height,
//                     startRow, numRows,
//                     args->maxIterations,
//                     args->output);

    // Load balanced
    for (int i = args->threadId; i < args->height; i += args->numThreads) {
        mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
                 args->width, args->height,
                 i, 1,
                 args->maxIterations,
                 args->output);
    }

    double endTime = CycleTimer::currentSeconds();
//    printf("Thread %d: row (%d - %d)\t\ttime for execution is \t[%.3f]ms\n",
//           args->threadId, startRow, startRow + numRows, (endTime - startTime) * 1000);
    printf("Thread %d time for execution is\t[%.3f]ms\n",
           args->threadId, (endTime - startTime) * 1000);
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i=0; i<numThreads; i++) {
      
        // TODO FOR CS149 STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
      
        args[i].threadId = i;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i=1; i<numThreads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    
    workerThreadStart(&args[0]);

    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}

