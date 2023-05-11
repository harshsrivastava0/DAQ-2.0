# Motor Controller
Following this readme file, you'll be able to extract data from the DTI HV-500 LC motor controller starting from scratch. This Motor Controller uses CANBUS communication to send and receive data. We'll be seeing how to establish a connection between the Motor Controller and the CANBUS wiring, how to get the raw canbus data, and how to convert this raw data into useable form.

 
## Connections
Final Connection should look something like this: 
<img src="https://github.com/harshsrivastava0/Data-Acquisition-System/blob/master/Images/entire-mc-setup.jpeg?raw=true" alt="entire-mc-setup" width="300"/>

To achieve this:
1. Make your own canbus wiring with two identical wires twisted in a loop. Make sure to attach two 120 ohm resistors on each end.
2. Mark one of these wires as CAN high and the other one as CAN low.
3. To the CAN high wire, connect the wire coming in from PIN-12 of the Motor Controller's Harness Connector. This pin is marked for CAN2.0 high.
4. To the CAN low wire, connect the wire coming in from PIN-4 of the Motor Controller's Harness Connector. This pin is marked for CAN2.0 low.
<img src="https://github.com/harshsrivastava0/Data-Acquisition-System/blob/master/Images/mc-harness-connector.png?raw=true" alt="mc-harness-connector" width="300"/>

5. To the CAN high wire, also connect the wire coming in from MCP2515 module's CAN-H pin.
6. To the CAN low wire, also connect the wire coming in from MCP2515 module's CAN-L pin.
7. Then connect the MCP2515 to the Arduino Mega 2560 Board using the following connections: 
<img src="https://github.com/harshsrivastava0/Data-Acquisition-System/blob/master/Images/megatomcp.jpg?raw=true" alt="megatomcp" width="300"/>
This MCP2515 module is used to communicate the raw canbus data to the arduino board via SPI communication protocol.

8. Now connect the Arduino board to your laptop using the Type-A to Type-B connector.
And voila! you're good to go.

## Getting the raw CANBUS data
For this achieve a set of pre-requisites:
1. Download the latest version of arduino IDE on your laptop.
2. On your personal laptop (having the arduino IDE), install the libraries:
	1. arduino-mcp2515 (https://github.com/autowp/arduino-mcp2515/archive/master.zip)
	2. mcp-can.h (https://downloads.arduino.cc/libraries/github.com/coryjfowler/mcp_can-1.5.0.zip)
3. In the arduino IDE, go to Sketch -> Include Library -> Add .ZIP Library and add the .zip files for both the libararies you just downloaded.
4. 
