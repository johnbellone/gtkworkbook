#!/usr/bin/env python
import sys
import os
import time
import optparse
import logging
import socket
import thread
import urllib

def acceptor(serversock, descriptors):
    while 1:
        # Waiting for connection...
        clientsock,addr = serversock.accept()
        descriptors.append((clientsock,addr))
        time.sleep(1/100)
        
def main():
    HOST = ''
    PORT = 50000
    MAXPENDING = 5
    URI = "http://finance.yahoo.com/d/quotes.csv?s=JAVA+C+AAPL+MSFT&f=snl1c6ahg"
    
    serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
    serversock.bind((HOST,PORT));
    serversock.listen(MAXPENDING);
    
    descriptors = []

    thread.start_new_thread(acceptor, (serversock, descriptors))
    
    running = 1

    while running:
        csv = urllib.urlopen(URI)
        blob = csv.read()
        
        # Iterate through the clients and print, print, print!
        for client in descriptors:
            client[0].send(blob)

        time.sleep(2)

# Execute the main method of the script.
if __name__ == "__main__":
    main()

# Copyright (C) 2009, John Bellone, Jr. <jvb4@njit.edu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy olf the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
