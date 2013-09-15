/* 
 * there are several arguments of this little benchmark
 * mem_len:          the size of the array in bytes. It is also the size of memory allocated
 * op_size:    the operation size of a single operation
 * start_addr: the address in byte where we start our operations
 * stride:     how far we jump between two operation addresses, it can be negative so it jumps back
 *
 * THis program will print the following outputs:
 *  1. throughput: operations per second
 *  2. bandwith:   bytes per second
 *  3. total time in seconds
 *
 * */


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
    if ( argc != 5 ) {
        printf("Usage: %s mem_len start_addr op_size stride\n", argv[0]);
        exit(1);
    }

    long mem_len, op_size, start_addr, stride, n_loop;
    struct timeval start, end, result;
    double base_time, agg_time;
    char *pmem;

    /* initialize parameters */
    mem_len = atol(argv[1]);
    start_addr = atol(argv[2]);
    op_size = atol(argv[3]);
    stride = atol(argv[4]);
    n_loop = (mem_len-op_size) / abs(stride);

    pmem = (char *)malloc(mem_len);
    if ( pmem == NULL ) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(1);
    }

    /* 
     * To get a base line time 
     */
    gettimeofday(&start, NULL);

    /* The basic loop */
    long i;
    long op_addr;
    for ( i = 0; i < n_loop; ++i ) {
        op_addr = abs((start_addr + stride * i) % mem_len);
    }

    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    base_time = result.tv_sec + result.tv_usec/1000000.0;


    /* 
     * To get the aggerated time 
     */
    gettimeofday(&start, NULL);

    for ( i = 0; i < n_loop; ++i ) {
        op_addr = abs((start_addr + stride * i) % mem_len) ;
        (*(char *)(pmem+op_addr))++;
    }

    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    agg_time = result.tv_sec + result.tv_usec/1000000.0;


    /* results */
    double ops, bandwidth, op_duration;
    long noperations = n_loop;
    long nbytes = n_loop * op_size;
    op_duration = agg_time - base_time;
    
    ops = noperations / op_duration;
    bandwidth = nbytes / op_duration;



    /* clean up */
    free(pmem);

    /* Output header */
    printf("%15s %15s %15s %15s %15s %15s %15s %15s %15s %15s %15s %15s\n", 
            "mem_len", "start_addr", "op_size", "stride", "base_time", "agg_time", 
            "noperations", "nbytes", "op_duration", "ops", "bandwidth", 
            "HEADERMARKER_vm");
    printf("%15ld %15ld %15ld %15ld %15lf %15lf %15ld %15ld %15lf %15lf %15lf %15s\n", 
            mem_len, start_addr, op_size, stride, base_time, agg_time, 
            noperations, nbytes, op_duration, ops, bandwidth,
            "DATAMARKER_vm");
    return 0;
}

