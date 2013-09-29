import itertools,subprocess
import sys
from time import gmtime, strftime

def main():
    #justshow = True
    justshow = False 

    mem_len = range(12, 30) 
    mem_len = [2**x for x in mem_len]
    start_addr = [0]
    #stride = [1, 1024, 4*1024, 8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 256*1024, 512*1024]
    #stride = [1, 2, 512, 1024, 4*1024, 64*1024, 1024*1024, 2*1024*1024, 4*1024*1024, 8*1024*1024, 16*1024*1024]
    op_size = [1]
    stride = [12] 
    stride = [2**x for x in stride]
    print stride
    #stride = [1, 2, 512, 1024, 4*1024, 64*1024, 1024*1024, 2*1024*1024]
    #stride = stride + [-x for x in stride]

    parameters = [mem_len, start_addr, op_size, stride]
    paralist = list(itertools.product(*parameters))

    jobid = strftime("%Y-%m-%d-%H-%M-%S", gmtime())
    resultname = jobid + ".result"

    result_file = open(jobid+".result", 'w')
    for i in range(10):
        for para in paralist:
            para = list(para)
            for j in range(5):
                # skip invalid op_size and stride
                if para[2] > para[3]:
                    print "skip..", para[2], ">", para[3], "!"
                    continue
                if para[3] > para[0]:
                    print "skip..", para[3], ">", para[0], "!"
                    continue
                #if not ( para[2] == 1 or para[2] == para[3] ):
                #    continue

                parastr = [str(x) for x in para]
                cmd = ['./vmbench-touch'] + parastr
                print "repeat:", i, "j", j, cmd

                if not justshow:            
                    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
                    proc.wait()
                    for line in proc.stdout:
                        print line,
                        result_file.write(line+'\n')

    result_file.close()

if __name__ == "__main__":
    main()
