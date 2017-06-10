# Divergence Analysis
# A tool to detect divergence points on two execution traces
# Marcus Botacin - Unicamp - 2017
# This tool is part of the BranchMonitoringProject framework

# Receive line args
import sys
# For trace alignment
from alignment.sequence import Sequence
from alignment.vocabulary import Vocabulary
from alignment.sequencealigner import SimpleScoring, GlobalSequenceAligner

# alignment to list function
def align_to_list(align,start,end,align_factor):
    s=align[start:end].strip().replace(" ","").replace("-","----")
    s = [s[n:n+align_factor] for n in range(0, len(s), align_factor)]
    return s

def align(trace1,trace2):

    # Create sequences to be aligned.
    a = Sequence(trace1)
    b = Sequence(trace2)

    # Create a vocabulary and encode the sequences.
    v = Vocabulary()
    aEncoded = v.encodeSequence(a)
    bEncoded = v.encodeSequence(b)

    # Create a scoring and align the sequences using global aligner.
    scoring = SimpleScoring(2, -1)
    aligner = GlobalSequenceAligner(scoring, -2)
    score, encodeds = aligner.align(aEncoded, bEncoded, backtrace=True)

    # Iterate over optimal alignments and print them.
    for encoded in encodeds:
        alignment = v.decodeSequenceAlignment(encoded)
        align=str(alignment)

    # Convert aligned sequences into list
    # escaping multiple characters

    seq_size = len(align)
    half_size = seq_size/2

    # First Half
    s1 = align_to_list(align,0,half_size,4)

    # Second Half
    s2 = align_to_list(align,half_size,seq_size,4)

    # return the lists as result
    return s1,s2

# Some auxiliary functions below are very simple
# However, their purpose is to make code more readable
# so you can understand the algorithm just looking to their names

# emit convergence message
def emit_convergence():
    print("    \\ /")

# emit divergence message
def emit_divergence():
    print("    / \\")

# emit aligned message
def emit_aligned(i_seq1,i_seq2):
    print(i_seq1+" | "+i_seq2)

# check we are not handling empty values
def is_empty(elem1,elem2):
    if elem1==' ' or elem2==' ':
        return True
    return False

# check elements are aligned and the same
def check_equal(elem1,elem2):
    if elem1 == elem2:
        return True
    return False

# check if execution already diverged
def already_diverged(diverged):
    return diverged

# change divergence status
def change_divergence(diverged):
    return not diverged

# given two lists of aligned executions
# identify divergence/convergence points
def check_divergence(seq1,seq2):
    seq1_index=0
    seq2_index=0
    diverged=False
    while seq1_index < len(seq1):
            s1_elem=seq1[seq1_index]
            s2_elem=seq2[seq2_index]
            if not is_empty(s1_elem,s2_elem):
                    if check_equal(s1_elem,s2_elem):
                            if already_diverged(diverged):
                                    emit_convergence()
                                    # it assumes a 1-level divergence
                                    # if you want to handle multi-level cases
                                    # change this from boolean to a counter
                                    # and add/sub accordingly
                                    diverged=change_divergence(diverged)
                    else:
                            if not already_diverged(diverged):
                                    emit_divergence()
                                    # the same for this point
                                    diverged=change_divergence(diverged)
                    emit_aligned(seq1[seq1_index],seq2[seq2_index])
            seq1_index+=1
            seq2_index+=1

# print usage information
def usage(bin_name):
    print("Usage: python %s <first_trace> <second_trace>" % bin_name)

# parse line args
def parse(bin_name,args):
    # accept only two line args
    if(len(args)!=2):
        # case not: display usage message
        usage(bin_name)
        # and quit
        sys.exit(0)
    # case ok, split args into trace1 and trace2
    return args[0],args[1]

# get traces on the proper format
def get_trace(trace_file):
    return open(trace_file,"r").read().strip().split("\n")

# main
def main(trace1,trace2):
    reference_trace = get_trace(trace1)
    compared_trace = get_trace(trace2)
    seq1,seq2 = align(reference_trace,compared_trace)
    check_divergence(seq1,seq2)

# entry point
if __name__ == "__main__":
    trace1,trace2 = parse(sys.argv[0],sys.argv[1:])
    main(trace1,trace2)
# Only standalone use
else:
    print("No module import support yet")
