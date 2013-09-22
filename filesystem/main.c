/* 
 * This benchmark test the READ performance of a file system.
 * It creates files and measures the performance of reading them.
 *
 * there are several arguments of this little benchmark
 * ndir: number of direcotries
 * nfile_per_dir: use 1 here for bad performance
 * nops_per_file: number of operations. It is number of r/w applied to a 
 *       single file
 * size_per_op: in bytes
 * do_fsync: [0|1] whether or not do fsync() after EACH write. Doing this 
 *           will ruin the delayed allocation in Ext4 and creates more 
 *           fragmentation.
 * r_file_stride: if it is 4, the benchmark will read file00 and file04, 
 *                skipping 3 files in the middle
 * do_write: [0|1] sometimes we only want to create the files
 * do_read:  [0|1] soemtimes we only want to read the files 
 *
 ***********************************************************************************
 * ***** All arguments must be provided ! I don't want to do argument parsing ******
 ***********************************************************************************
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
#include <string.h>

int main(int argc, char **argv)
{
    if ( argc != 10 ) {
        printf("Usage: %s ndir nfile_per_dir nops_per_file size_per_op do_fsync "
               "r_file_stride do_write do_read topdir\n", argv[0]);
        exit(1);
    }

    /* initialize parameters */
    int ndir, nfile_per_dir, nops_per_file, size_per_op, do_fsync, r_file_stride, do_write, do_read;
    char topdir[1024]; /* use path longer than this? crash! */

    ndir = atoi(argv[1]);
    nfile_per_dir = atoi(argv[2]);
    nops_per_file = atoi(argv[3]);
    size_per_op = atoi(argv[4]);
    do_fsync = atoi(argv[5]);
    r_file_stride = atoi(argv[6]);
    do_write = atoi(argv[7]);
    do_read = atoi(argv[8]);
    strcpy(topdir, argv[9]);


    /* print out results */
    printf(
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s \n",
            "ndir", 
            "nfile_per_dir", 
            "nops_per_file", 
            "size_per_op", 
            "do_fsync", 
            "r_file_stride",
            "do_write", 
            "do_read",
            "topdir",
            "HEADERMARKER_fs"
            );
    printf(
            "%15d "
            "%15d "
            "%15d "
            "%15d "
            "%15d "
            "%15d "
            "%15d "
            "%15d "
            "%15s "
            "%15s \n",
            ndir, 
            nfile_per_dir, 
            nops_per_file, 
            size_per_op, 
            do_fsync, 
            r_file_stride,
            do_write, 
            do_read,
            topdir,
            "DATAMARKER_fs"
          );
    return 0;
}





