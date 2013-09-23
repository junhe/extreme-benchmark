import itertools,subprocess
import sys
from time import gmtime, strftime

def seg_fault(addr, doit):
    cmd = ['./seg_disturbor', addr, doit]
    cmd = [str(x) for x in cmd]
    print cmd
    proc = subprocess.Popen(cmd)
    proc.wait()

def main():
    mem_len = [1024*1024*1024, 2*1024*1024*1024]
    #mem_len = [1024*1024]
    start_addr = [0]
    #stride = [1, 1024, 4*1024, 8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 256*1024, 512*1024]
    #stride = [1, 2, 512, 1024, 4*1024, 64*1024, 1024*1024, 2*1024*1024, 4*1024*1024, 8*1024*1024, 16*1024*1024]
    op_size = [4096]
    stride = [4096*8]
    do_seg = [0,1]
    #stride = [1, 2, 512, 1024, 4*1024, 64*1024, 1024*1024, 2*1024*1024]
    #stride = stride + [-x for x in stride]

    parameters = [mem_len, start_addr, op_size, stride, do_seg]
    paralist = list(itertools.product(*parameters))

    jobid = strftime("%Y-%m-%d-%H-%M-%S", gmtime())
    resultname = jobid + ".result"

    result_file = open(jobid+".result", 'w')
    for i in range(5):
        for para in paralist:
            para = list(para)

            # skip invalid op_size and stride
            if para[2] > para[3]:
                continue

            para = [str(x) for x in para]
            cmd = ['./vmbench'] + para
            print cmd

            proc = subprocess.Popen(cmd[0:5], stdout=subprocess.PIPE)

            while proc.poll() == None:
                for i in range(1024):
                    seg_fault(i*4096, cmd[5]) 

            proc.wait()
            for line in proc.stdout:
                print line
                elems = line.split()
                if "HEADERMARKER" in line:
                    elems.append("with_seg_faults")
                elif "DATAMARKER" in line:
                    elems.append(cmd[5])
                else:
                    continue
                line = " ".join([str(x) for x in elems])
                result_file.write(line+'\n')

    result_file.close()

if __name__ == "__main__":
    main()
