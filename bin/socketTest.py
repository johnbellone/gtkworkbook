#!/usr/bin/env python
import sys
import optparse
import socket

def main():
    p = optparse.OptionParser()
    p.add_option("--port", "-p", default=8888)
    p.add_option("--input", "-i", default="test.txt")
    options, arguments = p.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("localhost", options.port))
    
    fp = open(options.input, "r")

    ii = 0

    sock.sendall ("^0^1^sheet1^1000000^3\n")
    
    while ii < 1000000:
        sock.sendall ("^%d^0^sheet1^%d^0^^0\n" %(ii, ii))
        ii = ii + 1

    sock.close()
        
if __name__ == '__main__':
    main()
