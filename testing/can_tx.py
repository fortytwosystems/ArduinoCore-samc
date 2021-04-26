import getopt
import sys
import subprocess

tx_target = 'Frame ID: 0064, Data: 66 55 44 33 22 11 00'
tx_target_count = 5 # looks for at least 5 instances, Arduino should be transmitting at short intervals

def can_tx(portnumber):
    print('--------------------------------------------')
    print('Checking CAN transmit\n')

    ret = 0

    result = ''
    try:
        result = subprocess.run(['./usb-can/canusb', '-t', '-d', portnumber, '-s', '125000', '-b', '2000000', '-n', '20'], timeout = 10, capture_output=True, text=True).stdout
    except subprocess.TimeoutExpired:
        pass

    print('Looking for \'' +tx_target+ '\' in...\n')
    print(result)
    if result.count(tx_target) >= tx_target_count:
        print('\tFOUND - SUCCESS\n')
    else:
        print('\tNOT FOUND - FAILURE\n')
        ret = -1
    print('--------------------------------------------')

    return ret

if __name__ == "__main__":
    can_tx('/dev/ttyUSB0')