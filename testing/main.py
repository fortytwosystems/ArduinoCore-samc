import can_tx

def main():
    portnumber = ''
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'p:')
    except getopt.GetoptError:
        sys.exit(1)
    for opt, arg in opts:
        if opt == "-p":
            portnumber = arg
    
    if portnumber == '':
        print("No serial port specified")
        return
    
    check_count = 0

    check_count += can_tx(portnumber)

if __name__ == "__main__":
    main()