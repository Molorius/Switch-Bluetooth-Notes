#!/usr/bin/env python3

import sys
import bluetooth


def main():
    if len(sys.argv) < 2:
        print("Give the bluetooth address you want to connect to.")
        return
    addr = sys.argv[1]  # get the bluetooth address
    sock = bluetooth.BluetoothSocket(bluetooth.L2CAP)  # make an l2cap socket
    sock.bind((addr, 0x11))
    sock.close()


if __name__ == "__main__":
    main()
