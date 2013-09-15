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
    n_loop = (mem_len-op_size) / stride;

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
        op_addr = start_addr + stride * i;
    }

    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    base_time = result.tv_sec + result.tv_usec/1000000.0;


    /* 
     * To get the aggerated time 
     */
    gettimeofday(&start, NULL);

    for ( i = 0; i < n_loop; ++i ) {
        op_addr = start_addr + stride * i;
        (*(char *)(pmem+op_addr))++;
    }

    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    agg_time = result.tv_sec + result.tv_usec/1000000.0;


    /* clean up */
    free(pmem);

    /* Output header */
    printf("%20s %20s %20s %20s %20s %20s %20s\n", 
            "mem_len", "start_addr", "op_size", "stride", "base_time", "agg_time", "HEADERMARKER_vm");
    printf("%20ld %20ld %20ld %20ld %20lf %20lf %20s\n", 
            mem_len, start_addr, op_size, stride, base_time, agg_time, "DATAMARKER_vm");
    return 0;
}

