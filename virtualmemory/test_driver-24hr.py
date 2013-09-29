import itertools,subprocess
import sys
from time import gmtime, strftime

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

    mem_len = [ 4*1024*1024*1024 ]
    start_addr = [0]
    #stride = [1, 1024, 4*1024, 8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 256*1024, 512*1024]
    #stride = [1, 2, 512, 1024, 4*1024, 64*1024, 1024*1024, 2*1024*1024, 4*1024*1024, 8*1024*1024, 16*1024*1024]
    exps = [16] 
    op_size = [2**x for x in exps]
    stride = [2**x for x in exps]
    print stride
    #stride = [1, 2, 512, 1024, 4*1024, 64*1024, 1024*1024, 2*1024*1024]
    #stride = stride + [-x for x in stride]

    parameters = [mem_len, start_addr, op_size, stride]
    paralist = list(itertools.product(*parameters))

    jobid = strftime("%Y-%m-%d-%H-%M-%S", gmtime())

    result_file = open("h2.24hr/"+jobid+".result", 'w')
    while True:
        for para in paralist:
            para = list(para)

            # skip invalid op_size and stride
            if para[2] > para[3]:
                continue
            #if not ( para[2] == 1 or para[2] == para[3] ):
            #    continue

            para = [str(x) for x in para]
            cmd = ['./vmbench'] + para
            print cmd

            if not justshow:            
                proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
                proc.wait()
                for line in proc.stdout:
                    finish_time = strftime("%Y-%m-%d-%H-%M-%S", gmtime())
                    line = decrate(line, "finish_time", finish_time)
                    print line,
                    result_file.write(line)
                    result_file.flush()

    result_file.close()

if __name__ == "__main__":
    main()
