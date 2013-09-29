import itertools,subprocess
import sys
from time import localtime, strftime, sleep
import FormatFS

# In this test, I write with very small amount of
# data in each operation. 
# A: append-close-append-close...: measure the read bw.
# B: write in small size-close: measure the read bw. 
# Hopefully there will be fragmentations that affects 
# the bw. 

def trimcmd(cmd):
    "use this when printing and Popen"
    return cmd[0:10]

def decrate(line, key, val):
    elems = line.split() 
    if "HEADERMARKER" in line:
        elems.append(key)
    elif "DATAMARKER" in line:
        elems.append(val)
    else:
        return line

    return " ".join([str(x).rjust(18) for x in elems]) + "\n"

        

def main():
    #justshow = True
    justshow = False

    ndir = [1] # 1
    nfile_per_dir = [1] #2
    nops_per_file = [0] #3, decided by totalbytes
    size_per_op = [1] #4 
    do_fsync = [0] #5
    do_write = [0] #6
    do_read = [0]  #7
    topdir = ["/l0"] #8 
    do_append = [1] #9
    totalbytes = [128*1024*4*1024] #10
    switchcpu = [0]

    NDIR = 1
    NFILE_PER_DIR = 2
    NOPS_PER_FILE = 3
    SIZE_PER_OP = 4
    DO_FSYNC = 5
    DO_WRITE = 6
    DO_READ = 7
    TOPDIR = 8
    DO_APPEND = 9
    TOTALBYTES = 10
    SWITCHCPU = 11
    
    parameters = [ndir, nfile_per_dir, nops_per_file, 
                  size_per_op, do_fsync, do_write, do_read, topdir, 
                  do_append, totalbytes, switchcpu]
    paralist = list(itertools.product(*parameters))

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
            if not justshow:
                FormatFS.remakeExt4(partition, cmd[8], "jhe", "plfs", 
                    blockscount=2*1024*1024, blocksize=4096)
                subprocess.Popen("echo 262144 > /sys/fs/ext4/sda4/mb_stream_req", 
                        shell=True)
                sleep(3)

            #########################
            # Create files for later reads
            filesize = cmd[TOTALBYTES] / (cmd[NDIR]*cmd[NFILE_PER_DIR])
            opsize = min(4096, filesize) # in case the file is too small, like 1 byte
            _nops_per_file = filesize / opsize
            if _nops_per_file < 1:
                # we don't accept number that is not 2^x
                continue
            cmd[SIZE_PER_OP] = opsize
            cmd[NOPS_PER_FILE] = _nops_per_file

            cmd[DO_WRITE] = 1
            cmd[DO_READ] = 0

            if cmd[SWITCHCPU] == 0:
                # do not switch cpu
                cmd[DO_APPEND] = 1
                cmd[DO_FSYNC] = 0 
                print "repeat:", _nops_per_file, \
                        "For writing NOSWITCH (append):", trimcmd(cmd)
                for i in range(_nops_per_file):
                    cmd[NOPS_PER_FILE] = 1
                    cmd = [str(x) for x in cmd]
                    if not justshow:
                        proc = subprocess.Popen(trimcmd(cmd), 
                                stdout=subprocess.PIPE)
                        proc.wait()

            else:
                # do the appending thing
                # size per operation does not change, 
                # we just do one operation per file open
                cmd[DO_APPEND] = 1
                cmd[DO_FSYNC] = 1
                print "repeat:", _nops_per_file, \
                        "For writing (append):", trimcmd(cmd)
                for i in range(_nops_per_file):
                    cmd[NOPS_PER_FILE] = 1
                    cmd = [str(x) for x in cmd]
                    taskset_wrapper = ["taskset", "0x0000000"+str((i%2) + 1)]
                    #print taskset_wrapper + trimcmd(cmd) 

                    if not justshow:
                        proc = subprocess.Popen(taskset_wrapper+trimcmd(cmd), 
                                stdout=subprocess.PIPE)
                        proc.wait()

            ########################
            # Read the files in different ways, re-mount before 
            # each reading


            opsizes = sorted([4096], reverse=True)
            
            # do read for each operation size if it is valid
            for _size_per_op in opsizes:
                #re-mount the file system to drop caches
                if not justshow:
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
                print "For read:", trimcmd(cmd)

                # Run it
                if not justshow:
                    proc = subprocess.Popen(trimcmd(cmd), stdout=subprocess.PIPE)
                    proc.wait()
                    for line in proc.stdout:
                        line = decrate(line, "switchcpu", cmd[SWITCHCPU])
                        print line,
                        result_file.write(line)

    result_file.close()

if __name__ == "__main__":
    main()


