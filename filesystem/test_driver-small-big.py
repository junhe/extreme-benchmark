import itertools,subprocess
import sys
from time import localtime, strftime
import FormatFS

def main():

    # ndir
    exps = [5] 
    ndir = [2**x for x in exps] # 1
    
    # nfile_per_dir
    exps = [5]
    nfile_per_dir = [2**x for x in exps] #2

    # nops_per_file
    nops_per_file = [0] #3, decided by totalbytes
    #size_per_op = [1, 1024, 4096, 1024*1024, 4*1024*1024], reverse=True) #4 

    # others
    size_per_op = [0] #4 
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
    for rep in range(5):
        for para in paralist:
            para = list(para)
            cmd = ['./fsbench'] + para
        
            filesizes = range(64-4, 64+4)
            filesizes = [x*1024 for x in filesizes]

            for filesize in filesizes:    
                #########################
                # get a clean file system
                #FormatFS.remakeExt4(partition, cmd[8], "jhe", "plfs", 
                    #blockscount=2*1024*1024, blocksize=4096)

                #########################
                # Create files for later reads
                cmd[SIZE_PER_OP] = filesize
                cmd[NOPS_PER_FILE] = 1

                cmd[DO_WRITE] = 1
                cmd[DO_READ] = 0
                cmd = [str(x) for x in cmd]
                print "For writing:", cmd

                #proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
                #proc.wait()


                ########################
                # Read the files in different ways, re-mount before 
                # each reading

                #re-mount the file system to drop caches
                #FormatFS.umountFS(cmd[8])
                #FormatFS.mountExt4(partition, cmd[8])
                
                cmd[DO_WRITE] = 0
                cmd[DO_READ] = 1
                # make the total amount of bytes read the same
                cmd[SIZE_PER_OP] = min(filesizes) 

                cmd = [str(x) for x in cmd]
                print "For read:", cmd

                # Run it
                #proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
                #proc.wait()
                #for line in proc.stdout:
                    #print line,
                    #result_file.write(line)

    result_file.close()

if __name__ == "__main__":
    main()


