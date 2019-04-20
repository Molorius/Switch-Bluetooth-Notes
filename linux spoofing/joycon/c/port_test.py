#!/usr/bin/env python3

import bluetooth

sock_ctrl = bluetooth.BluetoothSocket( bluetooth.L2CAP )
sock_intr = bluetooth.BluetoothSocket( bluetooth.L2CAP )

port_ctrl = 0x11
port_intr = 0x13

print("Opening ctrl port")
sock_ctrl.bind(("", port_ctrl))
sock_ctrl.listen(1)

sock_ctrl_client, address = sock_ctrl.accept()
print("Got connection on ctrl from", address)

print("Opening intr port")
sock_intr.bind(("", port_intr))
sock_intr.listen(1)

sock_intr_client, address = sock_intr.accept()
print("Got connection on intr from", address)

print("Closing all ports")
sock_intr_client.close()
sock_ctrl_client.close()
sock_intr.close()
sock_ctrl.close()

