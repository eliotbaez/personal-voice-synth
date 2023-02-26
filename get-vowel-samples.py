#!/usr/bin/python3

# python script to quickly get voice sample data 
import sys
import os

def main():
    freq = int(sys.argv[1])
    inpath = sys.argv[2]
    outpath = sys.argv[3]
    samples = int(sys.argv[4])

    for v in "aeiou":
        infile = inpath + "/" + v + str(freq) + ".wav"
        outfile = outpath + "/" + v + str(freq) + ".csv"
        cmd = "./harmonic-finder %s %s %s >%s" % (infile, samples, freq, outfile)
        print(cmd)
        os.system(cmd)



if len(sys.argv) != 5:
    sys.stderr.write("Usage: %s FREQUENCY INPATH OUTPATH SAMPLES\n")
    exit(1)
if __name__ == "__main__":
    exit(main())