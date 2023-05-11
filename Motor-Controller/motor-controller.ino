#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>


//defining some variables
const int SPI_CS_PIN = 53;
unsigned char len = 8;
unsigned char buf[8] = {0};
long unsigned int rxId;
double mctempmotor = 0;
double mcerpm = 0;
double mcthrottle = 0;
double mctempcontroller = 0;
String mcfault = "Searching";


//defining CS pin
MCP_CAN CAN(SPI_CS_PIN);


//defining two funstions that convert decimal to hexadecimal and vice versa
int hexToDec(String hexString) {
  int decimalValue = 0;
  int len = hexString.length();
  for (int i = 0; i < len; i++) {
    char c = hexString.charAt(i);
    int digit;
    if (isDigit(c)) {
      digit = c - '0';
    } else {
      c = toupper(c);
      digit = c - 'A' + 10;
    }
    decimalValue += digit * pow(16, len - i - 1);
  }
  return decimalValue;
}


String decToHex(int decimalValue) {
  String hexString = "";
  while (decimalValue > 0) {
    int remainder = decimalValue % 16;
    char hexDigit;
    if (remainder < 10) {
      hexDigit = '0' + remainder;
    } else {
      hexDigit = 'A' + remainder - 10;
    }
    hexString = String(hexDigit) + hexString;
    decimalValue /= 16;
  }
  return hexString;
}


//setup
void setup()
{
  Serial.begin(9600);
  while (!Serial);

  if (CAN_OK != CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ))
  {
    Serial.println("CAN BUS FAIL!");
    while (1) delay(100);
  }
  else {
    Serial.println("CAN BUS OK!");
    delay(500);
  }
  CAN.setMode(MCP_NORMAL);
  pinMode(2, INPUT);

}


void loop()
{ 
  // check if data coming
  if (CAN_MSGAVAIL == CAN.checkReceive())
  {
    Serial.println("-----------------------------");
    Serial.println("CAN.checkReceive() is getting executed");
    if (!digitalRead(2)){
      //read data
      CAN.readMsgBuf(&rxId, &len, buf);
      Serial.println("CAN.readMsgBuf is getting executed");
    }
    
    long unsigned int rxId0 = rxId & 0x1FFFFFFF;
    long unsigned int Packet_id = (rxId0 >> 8);
    long unsigned int Node_id = (rxId0 & 0xFF);
    
    Serial.print("CAN ID: ");
    Serial.print(rxId, HEX);
    Serial.print("\t");
    Serial.print(rxId0, HEX);
    Serial.print("\t");
    Serial.print ("Packed Id: ");
    Serial.print (Packet_id, HEX);
    Serial.print ("\t");
    Serial.print ("Node Id: ");
    Serial.println(Node_id, HEX);
    
    
    //Converting decimal data into hexadecimal strings
    String hex1 = decToHex(buf[0]);
    String hex2 = decToHex(buf[1]);
    String hex3 = decToHex(buf[2]);
    String hex4 = decToHex(buf[3]);
    String hex5 = decToHex(buf[4]);
    
    if (Packet_id == 2){
      
      //Motor Temperature
      String mctempmotorhex = hex3 + hex4;
      mctempmotor = 0.1*hexToDec(mctempmotorhex);
      
      //Controller Temperature
      String mctempcontrollerhex = hex1 + hex2;
      mctempcontroller = 0.1*hexToDec(mctempcontrollerhex);

      //Faults
      String mcfaulthex = hex5;
      if (mcfaulthex == "0"){
        mcfault = "No faults";
      }
      else if (mcfaulthex == "1"){
        mcfault = "Overvoltage Error";
      }
      else if (mcfaulthex == "2"){
        mcfault = "Undervoltage Error";
      }
      else if (mcfaulthex == "3"){
        mcfault = "DRV error";
      }
      else if (mcfaulthex == "4"){
        mcfault = "ABS. Overtemp.";
      }
      else if (mcfaulthex == "5"){
        mcfault = "Controller Overtemp.";
      }
      else if (mcfaulthex == "6"){
        mcfault = "Motor Overtemp.";
      }
      else if (mcfaulthex == "7"){
        mcfault = "Sensor Wire error";
      }
      else if (mcfaulthex == "8"){
        mcfault = "Sensor General Error";
      }
      else if (mcfaulthex == "9"){
        mcfault = "CAN command error";
      }
      else if (mcfaulthex == "A"){
        mcfault = "Analog input error";
      }
    }

    //ERPM
    if (Packet_id == 0){
      String mcerpmhex = hex1+hex2+hex3+hex4;
      mcerpm = hexToDec(mcerpmhex);
    }

    //Throttle
    if (Packet_id == 4){
      mcthrottle = buf[0];
    }
    
    
    //Printing Useable data
    Serial.print("Motor Temp = ");
    Serial.print(mctempmotor);
    Serial.println(" C");

    Serial.print("Controller Temp = ");
    Serial.print(mctempcontroller);
    Serial.println(" C");
    
    Serial.print("ERPM = ");
    Serial.println(mcerpm);

    Serial.print("Throttle = ");
    Serial.println(mcthrottle);
    

    //Printing raw canbus data
    for (int i = 0; i < len; i++)
    {
      Serial.print(buf[i],HEX);   
      Serial.print("\t");
    }
    Serial.println();
    
    //giving delay
    delay(100);
  }
}
    
    

