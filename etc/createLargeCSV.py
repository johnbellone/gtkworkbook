#!/usr/bin/env python
import sys
import os
import time
import optparse
import random

def main():
    p = optparse.OptionParser()
    p.add_option("--output", "-o", default="largefile.csv")
    p.add_option("--rows", "-r", default=10000000)
    p.add_option("--columns", "-c", default=20)
    options, arguments = p.parse_args()

    fp = open(options.output, "w")
    ii = 0 
    jj = 0
    maxRows = int(options.rows)
    maxCols = int(options.columns)

    while ii < maxRows:
        fp.write(str(ii+1) + ",")
        jj = 1
        while jj < maxCols-1:
            choice = random.choice([1,2,3,4,5])

            if choice == 1:
                fp.write("ABCDEFG,")
            elif choice == 2:
                fp.write("1234567,")
            elif choice == 3:
                fp.write("HIJKLMN,")
            elif choice == 4:
                fp.write("8901234,")
            elif choice == 5:
                fp.write("OPQRSTU,")
            jj = jj+1
        fp.write("VWXYZ567890\r\n")
        fp.flush()
        ii = ii+1
    fp.close()

if __name__ == "__main__":
    main()
