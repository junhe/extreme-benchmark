import itertools,subprocess
import sys
from time import localtime, strftime
import FormatFS

def main():
    totalbytes = 1*1024*1024*1024

    exps = range(1)
    ndir = [2**x for x in exps] # 1

    exps = range(1)
    nfile_per_dir = [2**x for x in exps] #2
    nops_per_file = [0] #3, decided by totalbytes
    #size_per_op = [1, 1024, 4096, 1024*1024, 4*1024*1024], reverse=True) #4 
    size_per_op = [1] #4 
    do_fsync = [0] #5
    do_write = [0] #6
    do_read = [0]  #7
    topdir = ["/l0"] #8 

    NDIR = 1
    NFILE_PER_DIR = 2
    NOPS_PER_FILE = 3
    SIZE_PER_OP = 4
    DO_FSYNC = 5
    DO_WRITE = 6
    DO_READ = 7
    TOPDIR = 8
    
    parameters = [ndir, nfile_per_dir, nops_per_file, 
                  size_per_op, do_fsync, do_write, do_read, topdir]
    paralist = list(itertools.product(*parameters))

    partition = "/dev/sda4"

    jobid = strftime("%Y-%m-%d-%H-%M-%S", localtime())
    resultname = jobid + ".result"

    result_file = open(jobid+".result", 'w')
    for para in paralist:
        para = list(para)
        cmd = ['./fsbench'] + para

        # decide nops_per_file
        _nops_per_file = \
                totalbytes/(cmd[NDIR]*cmd[NFILE_PER_DIR]*cmd[SIZE_PER_OP])
        if _nops_per_file < 1:
            continue
        cmd[NOPS_PER_FILE] = _nops_per_file
        cmd = [str(x) for x in cmd]
        #print cmd
        #continue

        # get a clean file system
        FormatFS.remakeExt4(partition, cmd[8], "jhe", "plfs", 
            blockscount=2*1024*1024, blocksize=4096)

        # write the files
        cmd[6] = "1" # do_write
        cmd[7] = "0" # do_read
        print cmd

        #proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        #proc.wait()

        # re-mount the file system to drop caches
        FormatFS.umountFS(cmd[8])
        FormatFS.mountExt4(partition, cmd[8])

       
        # read the files
        cmd[6] = "0" # do_write
        cmd[7] = "1" # do_read
        print cmd

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        proc.wait()
        for line in proc.stdout:
            print line,
            result_file.write(line)

    result_file.close()

if __name__ == "__main__":
    main()


