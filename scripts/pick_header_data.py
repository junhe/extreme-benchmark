import sys

# print out
# xxx  xxx xxx  HEADERMARKER_
# xxx  xxx xxx  DATAMARKER_
# xxx  xxx xxx  DATAMARKER_
# xxx  xxx xxx  DATAMARKER_
def pick_header_data_lines(fpath):
    headerprinted = False
    with open(fpath, 'r') as f:
        for line in f:
            if "HEADERMARKER_" in line and headerprinted == False:
                print line,
                headerprinted = True
            elif "DATAMARKER_" in line:
                print line,
            else:
                pass

pick_header_data_lines( sys.argv[1] )
