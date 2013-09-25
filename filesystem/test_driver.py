import itertools,subprocess
import sys
from time import localtime, strftime
import FormatFS

def main():
    totalbytes = 1*1024*1024*1024

    # ndir
    exps = [0, 1, 9] 
    #exps = [9] 
    ndir = [2**x for x in exps] # 1
    
    # nfile_per_dir
    exps = [0, 1, 9]
    #exps = [9]
    nfile_per_dir = [2**x for x in exps] #2

    # nops_per_file
    nops_per_file = [0] #3, decided by totalbytes
    #size_per_op = [1, 1024, 4096, 1024*1024, 4*1024*1024], reverse=True) #4 

    # others
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
    
    #parameters = [ndir, nfile_per_dir, nops_per_file, 
    #              size_per_op, do_fsync, do_write, do_read, topdir]
    #paralist = list(itertools.product(*parameters))
    paralist = [ \
                 [ 1, 1, 1048576, 1024, 0, 0, 0, '/l0'],
                 [ 512, 512, 4096, 1, 0, 0, 0, '/l0'] \
               ]

    partition = "/dev/sda4"

    jobid = strftime("%Y-%m-%d-%H-%M-%S", localtime())
    resultname = jobid + ".result"

    result_file = open(jobid+".result", 'w')
    for rep in range(5):
        for para in paralist:
            para = list(para)
            cmd = ['./fsbench'] + para
        
            #########################
            # get a clean file system
            FormatFS.remakeExt4(partition, cmd[8], "jhe", "plfs", 
                blockscount=2*1024*1024, blocksize=4096)

            #########################
            # Create files for later reads
            filesize = totalbytes / (cmd[NDIR]*cmd[NFILE_PER_DIR])
            opsize = min(4096, filesize) # in case the file is too small, like 1 byte
            _nops_per_file = filesize / opsize
            if _nops_per_file < 1:
                # we don't accept number that is not 2^x
                continue
            cmd[SIZE_PER_OP] = opsize
            cmd[NOPS_PER_FILE] = _nops_per_file

            cmd[DO_WRITE] = 1
            cmd[DO_READ] = 0
            cmd = [str(x) for x in cmd]
            print "For writing:", cmd


            cmd[NOPS_PER_FILE] = _nops_per_file
            cmd = [str(x) for x in cmd]

            #print cmd
            #continue

            proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
            proc.wait()


            ########################
            # Read the files in different ways, re-mount before 
            # each reading


            opsizes = sorted([1, 1024, 4*1024], reverse=True)
            
            # do read for each operation size if it is valid
            for _size_per_op in opsizes:
                if cmd[NDIR] == "512" and _size_per_op != 1:
                    continue
                if cmd[NDIR] == "1" and _size_per_op not in [1024, 4096]:
                    continue
                #re-mount the file system to drop caches
                FormatFS.umountFS(cmd[8])
                FormatFS.mountExt4(partition, cmd[8])

                _nops_per_file = filesize / _size_per_op

                if _nops_per_file < 1:
                    # we don't accept number that is not 2^x
                    continue
                cmd[NOPS_PER_FILE] = _nops_per_file
                cmd[SIZE_PER_OP] = _size_per_op
                
                cmd[DO_WRITE] = 0
                cmd[DO_READ] = 1

                cmd = [str(x) for x in cmd]
                print "For read:", cmd

                # Run it
                proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
                proc.wait()
                for line in proc.stdout:
                    print line,
                    result_file.write(line)
                    result_file.flush()

    result_file.close()

if __name__ == "__main__":
    main()


