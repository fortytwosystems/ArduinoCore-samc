import getopt
import sys
import subprocess

def main():
    portnumber = ''
    try:
        opts, args = getopt.getopt(sys.argv[1:], '')
    except getopt.GetoptError:
        sys.exit(1)
    for opt, arg in opts:
        if opt == "-p":
            portnumber = arg
    
    if portnumber == '':
        print("No serial port specified")
        return
    portnumber_opt = '-d ' + portnumber

    result = subprocess.run(['./can-usb', '-t', '-s 125000', '-b 2000000', portnumber_opt], capture_output=True, text=True)
    
    print(result)


if __name__ == "__main__":
    main()