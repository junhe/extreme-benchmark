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
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if ( argc != 9 ) {
        printf("Usage: %s ndir nfile_per_dir nops_per_file size_per_op do_fsync "
               "do_write do_read topdir do_append\n", argv[0]);
        printf("do_append is only for write\n");
        exit(1);
    }

    
    /* timer */
    struct timeval start, end, result;
    double read_open_time, read_time, read_close_time;

    /* initialize parameters */
    int ndir, nfile_per_dir, nops_per_file, size_per_op, do_fsync, do_write, do_read, do_append;
    char topdir[1024]; /* use path longer than this? crash! */

    ndir = atoi(argv[1]);
    nfile_per_dir = atoi(argv[2]);
    nops_per_file = atoi(argv[3]);
    size_per_op = atoi(argv[4]);
    do_fsync = atoi(argv[5]);
    do_write = atoi(argv[6]);
    do_read = atoi(argv[7]);
    strcpy(topdir, argv[8]);
    do_append = atoi(argv[9]);
    
    int nfiles = nfile_per_dir * ndir;
    long totalbytes = size_per_op * nops_per_file * nfiles;
    long wtotal, rtotal;

    /* create the files */
    if ( do_write == 1 ) {
        int dirno;
        char *buf;
        buf = (char *)malloc(size_per_op);
        if (buf == NULL) {
            perror("Failed to allocate memory for buf");
            exit(1);
        }

        /* create directories */
        for (dirno = 0; dirno < ndir; dirno++) {
            char dirpath[1024];
            int ret;
            sprintf(dirpath, "%s/dir.%05d", topdir, dirno);
            /*printf("%s\n", dirpath);           */
            
            ret = mkdir(dirpath, 0777);
            if ( ret != 0 && errno != EEXIST ) {
                perror("Failed to create dir");
                exit(1);
            }

            /* crete files */
            int fileno;
            for ( fileno = 0; fileno < nfile_per_dir; fileno++ ) {
                char filepath[1024];
                int fd;
                sprintf(filepath, "%s/file.%05d", dirpath, fileno);
                /*printf("%s\n", filepath);*/
                int flag = O_WRONLY|O_CREAT;
                if ( do_append == 1 ) {
                    flag |= O_APPEND;
                }
                fd = open(filepath, flag, 0644);
                if ( fd == -1 && errno != EEXIST) {
                    perror("failed to create file");
                    exit(1);
                }

                /* write to file */
                int writeno;
                for ( writeno = 0; writeno < nops_per_file; writeno++) {
                    int ret = write(fd, buf, size_per_op);
                    wtotal += ret;
                    if ( do_fsync == 1 ) {
                        fsync(fd);
                    }
                }
                close(fd);
            }
        }
        free(buf);
        assert( wtotal == totalbytes );
    }

    /* read the files */
    if ( do_read == 1 ) {
        char *buf;
        buf = (char *)malloc(size_per_op);
        if (buf == NULL) {
            perror("Failed to allocate memory for buf");
            exit(1);
        }

        int fileno, dirno;
#define FDS_MAX 524288 
        int fds[FDS_MAX]; /* file descriptos in the order of operations */
        int i_fds = 0;
        
        /* open all files */

        gettimeofday(&start, NULL);
        for ( fileno = 0; fileno < nfile_per_dir; fileno++ ) {
            for ( dirno = 0; dirno < ndir; dirno++ ) {
                char filepath[1024];
                sprintf(filepath, "%s/dir.%05d/file.%05d", 
                                  topdir, dirno, fileno);
                /*printf("%s\n", filepath);*/
                int fd = open(filepath, O_RDONLY);
                if ( fd == -1 ) {
                    perror("Failed to open file for read");
                    exit(1);
                }
                assert( i_fds < nfiles);
                fds[i_fds] = fd;
                i_fds++;
            }/* dirno */
        }/* fileno */
        gettimeofday(&end, NULL);
        timersub( &end, &start, &result );
        read_open_time = result.tv_sec + result.tv_usec/1000000.0;


        /* read the files in the order of opening */
        int ifile;
        int op;
        rtotal = 0;
        gettimeofday(&start, NULL);
        for ( op = 0; op < nops_per_file; op++ ) {
            for ( ifile = 0; ifile < nfiles; ifile++ ) {
                int ret = read(fds[ifile], buf, size_per_op);
                rtotal += ret;
                assert(ret == size_per_op);
            }
        }
        gettimeofday(&end, NULL);
        timersub( &end, &start, &result );
        read_time = result.tv_sec + result.tv_usec/1000000.0;

        /* close files */
        gettimeofday(&start, NULL);
        for ( ifile = 0; ifile < nfiles; ifile++ ) {
            close(fds[ifile]);
        }
        gettimeofday(&end, NULL);
        timersub( &end, &start, &result );
        read_close_time = result.tv_sec + result.tv_usec/1000000.0;
        
        free(buf);
        assert( rtotal == totalbytes );
    }/* do_read */


    /* print out results */
    double totaltime = read_open_time + read_time + read_close_time;

    char str_read_open_time[128];
    char str_read_time[128];
    char str_read_close_time[128];
    char str_r_effective_bw[128];
    char str_r_bw[128];
    char str_r_effective_iops[128];

    if ( do_read == 1 ) {
        sprintf(str_read_open_time, "%lf", read_open_time);
        sprintf(str_read_time, "%lf", read_time);
        sprintf(str_read_close_time, "%lf", read_close_time);

        double r_effective_bw = totalbytes / totaltime;
        sprintf(str_r_effective_bw, "%lf", r_effective_bw);

        double r_bw = totalbytes / read_time;
        sprintf(str_r_bw, "%lf", r_bw);

        double r_effective_iops = (nfiles*2 + nops_per_file*nfiles)/totaltime;
        sprintf(str_r_effective_iops, "%lf", r_effective_iops);
    } else {
        sprintf(str_read_open_time, "NA");
        sprintf(str_read_time, "NA");
        sprintf(str_read_close_time, "NA");
        sprintf(str_r_effective_bw, "NA");
        sprintf(str_r_bw, "NA");
        sprintf(str_r_effective_iops, "NA");
    }


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
            "do_write", 
            "do_read",
            "topdir",
            "read_open_time",
            "read_time",
            "read_close_time",
            "r_bw",
            "r_effective_bw",
            "r_effective_iops",
            "nfiles",
            "totalbytes",
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
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15s "
            "%15d "
            "%15ld "
            "%15s\n",
            ndir, 
            nfile_per_dir, 
            nops_per_file, 
            size_per_op, 
            do_fsync, 
            do_write, 
            do_read,
            topdir,
            str_read_open_time,
            str_read_time,
            str_read_close_time,
            str_r_bw,
            str_r_effective_bw,
            str_r_effective_iops,
            nfiles,
            totalbytes,
            "DATAMARKER_fs"
          );
    return 0;
}





