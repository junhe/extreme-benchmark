import itertools,subprocess
import sys
from time import localtime, strftime
import FormatFS

def main():

    ndir = [8] # 1
    nfile_per_dir = [8] #2
    nops_per_file = [8] #3
    size_per_op = [1, 4096] #4 
    do_fsync = [0,1] #5
    do_write = [0] #6
    do_read = [0]  #7
    topdir = ["/l0"] #8 
    
    parameters = [ndir, nfile_per_dir, nops_per_file, 
                  size_per_op, do_fsync, do_write, do_read, topdir]
    paralist = list(itertools.product(*parameters))

    partition = "/dev/sda4"

    jobid = strftime("%Y-%m-%d-%H-%M-%S", localtime())
    resultname = jobid + ".result"

    result_file = open(jobid+".result", 'w')
    for para in paralist:
        para = list(para)
        para = [str(x) for x in para]
        cmd = ['./fsbench'] + para

        # get a clean file system
        FormatFS.remakeExt4(partition, cmd[8], "jhe", "plfs", 
            blockscount=1024*1024, blocksize=4096)

        # write the files
        cmd[6] = "1" # do_write
        cmd[7] = "0" # do_read
        print cmd

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        proc.wait()

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


