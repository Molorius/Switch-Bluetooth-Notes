#!/usr/bin/env python3

import bluetooth
import select

port_range = [0, 50]
socket_list = []
for p in range(*port_range):

    s = bluetooth.BluetoothSocket( bluetooth.L2CAP )
    try:
        s.bind(("", p))
        s.listen(5)
        socket_list.append(s)
        print("Listening on port %i" % p)
    except Exception as e:
        #print(e)
        pass

print("Ports all ready to listen")
read_list = socket_list.copy()
while True:
    print("loop")
    readable, writeable, errored = select.select(read_list, [], [])
    for s in readable:
        if s in socket_list:
            client_sock, address = s.accept()
            print("Got connection from", address)
            socket_list.remove(s)
        else:
            data = s.recv(1024)
            if data:
                print("Got data from client:", data)
            else:
                s.close()
                print("A client closed the connection")
                #read_list.remove(s)
                socket_list.append(s)
