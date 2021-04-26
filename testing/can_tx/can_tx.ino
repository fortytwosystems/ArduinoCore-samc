#include <fortytwo_can.h>

FORTYTWO_CAN can(ID_CAN0, 22, 23, 0, 0);

void setup() {
    uint8_t ret;

    SerialUSB.begin(115200);
    ret = can.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ);
    if(ret == CAN_OK) {
        SerialUSB.println("CAN initialized sucessfully");
    }
    else {
        SerialUSB.println("CAN failed to initialize. Error code: " + ret);
    }
}

void loop() {
    uint8_t ret;
    uint32_t id;
    uint8_t len;
    uint8_t buf[8];
    uint8_t i;

    ret = can.readMsgBuf(&id, &len, buf);

    if(ret == CAN_OK) {
        SerialUSB.print("Got a message from: ");
        SerialUSB.print(id);
        SerialUSB.print("  Length:  ");
        
    }
}