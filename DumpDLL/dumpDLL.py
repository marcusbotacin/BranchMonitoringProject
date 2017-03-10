# Marcus Botacin
# UNICAMP - 2017
# Parse DLL dumps to generate an offset-ordered list
# The list is used on introspection header files

# Import Block
import sys  # read from argv

# Config Block
N=9 # Block Size

#Code Block

filename=sys.argv[1]    # Input = argv[1]
tuple_list=[]           # empty list to start
with open(filename, 'r') as infile:
    lines = []          # block lines
    for line in infile: # for each line
        lines.append(line)  # group N lines
        if len(lines) > N:
            name=lines[1].strip().split(":")[1].strip() # parse name (regex?)
            offset=int(lines[3].strip().split(":")[1].strip(),16) # parse offset (regex?)
            tuple_list.append([name,offset]) # list entry
            lines = [] # start new block

# sort list on reverse order
# reverse order allows us to stop at the first address match
s_list=sorted(tuple_list,key=lambda x: x[1],reverse=True)

# Print results as you want
# You may want to include typedef struct{ } defs in order to speed up the header
# geneartion procedure
for line in s_list:
    print(line[0]),
    print(","),
    print(line[1])
