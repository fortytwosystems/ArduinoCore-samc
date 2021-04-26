CAN Testing (Linux Only)
=========================

To test CAN, connect the following:
 
 - USB-CAN analyzer (Seeed studio) to MCP2551 (or similar) transciever
 - Transciever TX/RX to FortyTwo Mega pins 4/5
 - Power transiever from Mega 5V and connect all grounds.

Compile the usb-can analyzer program as follows (linux only):

``` bash
cd ~
git clone https://github.com/kobolt/usb-can
cd ~/usb-can
gcc -o canusb canusb.c
```

