import itertools,subprocess
import sys
from time import gmtime, strftime



    #for rep in range(10):
        #for para in paralist:
            #mycmd = [exefile] + list(para)
            #proc = subprocess.Popen(mycmd,
                           #stdout=subprocess.PIPE,
                           #stderr=logf)
            #proc.wait()





def main():
    mem_len = [128*1024*1024, 1024*1024*1024]
    start_addr = [0]
    op_size = [1]
    #stride = [1, 1024, 4*1024, 8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 256*1024, 512*1024]
    stride = [1024, 4*1024, 64*1024, 1024*1024]
    stride = stride + [-x for x in stride]

    parameters = [mem_len, start_addr, op_size, stride]
    paralist = list(itertools.product(*parameters))

    jobid = strftime("%Y-%m-%d-%H-%M-%S", gmtime())
    resultname = jobid + ".result"

    result_file = open(jobid+".result", 'w')
    for para in paralist:
        para = list(para)
        para = [str(x) for x in para]
        cmd = ['./vmbench'] + para
        print cmd

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        for line in proc.stdout:
            print line,
            result_file.write(line+'\n')

        proc.wait()
    result_file.close()

if __name__ == "__main__":
    main()
