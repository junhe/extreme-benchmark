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
                printf("%s\n", filepath);
                
                fd = open(filepath, O_WRONLY|O_CREAT, 0644);
                if ( fd != 0 && errno != EEXIST) {
                    perror("failed to create file");
                    exit(1);
                }

                /* write to file */
                int writeno;
                for ( writeno = 0; writeno < nops_per_file; writeno++) {
                    write(fd, buf, size_per_op);
                    if ( do_fsync == 1 ) {
                        fsync(fd);
                    }
                }

                close(fd);
            }
        }
        free(buf);
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
        int nfiles = nfile_per_dir * ndir;
#define FDS_MAX 32768 
        int fds[FDS_MAX]; /* file descriptos in the order of operations */
        int i_fds = 0;
        
        /* open all files */
        for ( fileno = 0; fileno < nfile_per_dir; fileno++ ) {
            for ( dirno = 0; dirno < ndir; dirno++ ) {
                char filepath[1024];
                sprintf(filepath, "%s/dir.%05d/file.%05d", 
                                  topdir, dirno, fileno);
                printf("%s\n", filepath);
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

        /* read the files in the order of opening */
        int ifile;
        int op;
        for ( op = 0; op < nops_per_file; op++ ) {
            for ( ifile = 0; ifile < nfiles; ifile++ ) {
                read(fds[ifile], buf, size_per_op);
            }
        }

        /* close files */
        for ( ifile = 0; ifile < nfiles; ifile++ ) {
            close(fds[ifile]);
        }
        
        free(buf);
    }/* do_read */

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





