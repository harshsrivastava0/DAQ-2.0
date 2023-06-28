//Order of printing data:

//mcdata: mccurrent, motortemp, controllertemp, erpm, mcspeed, throttle
//bms data: packcurrent, packinstvoltage, stateofcharge, hightemp, lowtemp
//mfr data in L/minute
//mc fault
//bms fault

#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
#include<SoftwareSerial.h>
SoftwareSerial telemetrySerial(5,3); // Create a software serial object for telemetry transmission
//SoftwareSerial HC12(5,6);

struct TelemetryData { // Define a struct to hold the telemetry data
  double mctempmotor = 0;
  double mctempcontroller = 0;
  double mcerpm = 0;
  double mcthrottle = 0;
  double pack_current = 0;
  double pack_inst_voltage = 0;
  double state_of_charge = 0;
  double high_temp = 0;
  double low_temp = 0;
};

//defining some variables
const int SPI_CS_PIN = 10;
unsigned char len = 8;
unsigned char buf[8] = {0};
long unsigned int rxId;
double mccurrent = 0;
double mctempmotor = 0;
double mcerpm = 0;
double mcspeed = 0;
double mcthrottle = 0;
double mctempcontroller = 0;
String mcfault = "NO MC FAULT";

//BMS Variables
double pack_current = 0;
double pack_inst_voltage = 0;
double state_of_charge = 0;
double high_temp = 0;
double low_temp = 0;

//MFR Variables
volatile int sensor_frequency;
unsigned int water_minute;
unsigned char flowmeter = 4;
unsigned long present_time;
unsigned long closedlooptime;



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

void decToBinary(int decimalNum, int binaryArray[], int arraySize) {
  // Initialize binary array to all zeros
  for (int i = 0; i < arraySize; i++) {
    binaryArray[i] = 0;
  }

  // Convert decimal number to binary and store in array
  int i = 0;
  while (decimalNum > 0 && i < arraySize) {
    binaryArray[i] = decimalNum % 2;
    decimalNum /= 2;
    i++;
  }
}

//MFR Function
void flow(){
  sensor_frequency++;
}

//setup
void setup()
{
  //pinMode(3, OUTPUT);
  Serial.begin(9600);
  telemetrySerial.begin(9600);
  while (!Serial);

  if (CAN_OK != CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ))
  {
    //Serial.println("CAN BUS FAIL!");
    while (1) delay(100);
  }
  else {
    //Serial.println("CAN BUS OK!");
    delay(500);
  }
  //Serial.println("11");
  CAN.setMode(MCP_NORMAL);
  //Serial.println("22");
  pinMode(2, INPUT);
  pinMode(flowmeter,INPUT);
  attachInterrupt(0,flow,RISING);
  sei();
  present_time = millis();
  closedlooptime = present_time;
}

void loop()
{ 
  //digitalWrite(3, HIGH);
  // check if data coming
  if (CAN_MSGAVAIL == CAN.checkReceive())
  {
    if (1){
      //read data
      CAN.readMsgBuf(&rxId, &len, buf);
    }

    // for (int i=0; i<=7; i++){
    //   Serial.println(buf[i]);
    // }

    long unsigned int rxId0 = rxId & 0x1FFFFFFF;
    long unsigned int Packet_id = (rxId0 >> 8);
    long unsigned int Node_id = (rxId0 & 0xFF);

    //Converting decimal data into hexadecimal strings
    String hex1 = decToHex(buf[0]);
    String hex2 = decToHex(buf[1]);
    String hex3 = decToHex(buf[2]);
    String hex4 = decToHex(buf[3]);
    String hex5 = decToHex(buf[4]);
    

    //some variables
    int bms_fault_check = 0;

    //Converting data into useable form
    TelemetryData data;

    //MC Code
    if (Packet_id == 1){
      String mccurrenthex = hex1 + hex2;
      mccurrent = 0.1*hexToDec(mccurrenthex);
    }

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
      mcerpm = mcerpm/10;
      mcspeed = (2*3.141592*0.2286*0.001)*60*mcerpm;

    }

    //Throttle
    if (Packet_id == 4){
      mcthrottle = buf[0];
    }


    //BMS CODE
    if(rxId==2147485360){

      //CURRENT
      String c = hex1 + hex2;
      pack_current=(hexToDec(c))/10;
      
      //VOLTAGE
      String v = hex3+hex4;
      pack_inst_voltage=(hexToDec(v))/10;
      
      //STATE OF CHARGE      
      state_of_charge = buf[4]/2;
      
      //TEMPERATURE
      high_temp = buf[5];
      low_temp = buf[6];
    }

    data.mctempmotor = mctempmotor;
    data.mctempcontroller = mctempcontroller;
    data.mcerpm = mcerpm;
    data.mcthrottle = mcthrottle;
    data.pack_current = pack_current;
    data.pack_inst_voltage = pack_inst_voltage;
    data.state_of_charge = state_of_charge;
    data.high_temp = high_temp;
    data.low_temp = low_temp;
    
    byte dataBytes[sizeof(data)];
    memcpy(dataBytes, &data, sizeof(data));
      

    // Transmit the encapsulated telemetry data
    telemetrySerial.write(dataBytes, sizeof(dataBytes));
  
    //telemetrySerial.print(data.pack_current);
    //telemetrySerial.print(",");
    //telemetrySerial.print(data.pack_inst_voltage);
    //telemetrySerial.print(",");
    //telemetrySerial.print(data.state_of_charge);
    //telemetrySerial.print(",");
    //telemetrySerial.print(data.high_temp);
    //telemetrySerial.print(",");
    //telemetrySerial.print(data.low_temp);
  
  
   // Wait for 1 second before sending the next telemetry packet


    //Printing MC data
    Serial.print(mccurrent);
    Serial.print(",");

    Serial.print(mctempmotor);
    Serial.print(",");

    Serial.print(mctempcontroller);
    Serial.print(",");
    
    Serial.print(mcerpm);
    Serial.print(",");

    Serial.print(mcspeed);
    Serial.print(",");

    Serial.print(mcthrottle);
    Serial.print(",");

    //Printing BMS Data
    Serial.print(pack_current);
    Serial.print(",");

    Serial.print(pack_inst_voltage);
    Serial.print(",");

    Serial.print(state_of_charge);
    Serial.print(",");

    Serial.print(high_temp);
    Serial.print(",");

    Serial.print(low_temp);
    Serial.print(",");

    //Printing MFR data
    present_time = millis();
    if(present_time>=(closedlooptime+500)){
      closedlooptime = present_time;
      water_minute = (sensor_frequency/7.5);
      sensor_frequency = 0;
      Serial.print(water_minute,DEC);
      Serial.print(",");
    }else{
      Serial.print('0');
      Serial.print(",");
    }

    // //Printing mc fault
    Serial.print(mcfault);
    Serial.print(";");
    
    //BMS faults
    if (rxId==2147485361){
      
      //DTC FLAGS #1
      int a=(buf[0]);
      int flag1[8];
      decToBinary(a, flag1, 8);
    
      if(flag1[7]==1)
      {
        Serial.print("Pack Too Hot Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag1[6]==1)
      {
        Serial.print("Lowest Cell Voltage Too Low");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag1[5]==1)
      {
        Serial.print("Highest Cell Voltage Too High");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag1[4]==1)
      {
        Serial.print("Internal Software Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag1[3]==1)
      {
        Serial.print("Internal Heatsink Thermistor Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag1[2]==1)
      {
        Serial.print("Internal Hardware Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag1[1]==1)
      {
        Serial.print("Charger Safety Relay Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag1[0]==1)
      {
        Serial.print("Discharge Limit Enforcement Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      

      //DTC FLAG #2
      int b=(buf[2]);
      int flag2[8];
      decToBinary(b, flag2, 8);

      
      if(flag2[7]==1)
      {
        Serial.print("Cell ASIC Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag2[6]==1)
      {
        Serial.print("Highest Cell Voltage Over 5V Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag2[5]==1)
      {
        Serial.print("Current Sensor Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag2[4]==1)
      {
        Serial.print("Open Wiring Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag2[3]==1)
      {
        Serial.print("Low Cell Voltage Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag2[2]==1)
      {
        Serial.print("Weak Cell Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag2[1]==1)
      {
        Serial.print("Cell Balancing Stuck Off Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag2[0]==1)
      {
        Serial.print("Internal Communication Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      
      //DTC FLAG #3
      int c=(buf[3]);
      int flag3[8];
      decToBinary(c, flag3, 8);

      
      if(flag3[7]==1)
      {
        Serial.print("Charge Limit Enforcement Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag3[6]==1)
      {
        Serial.print("Input Power Supply Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag3[5]==1)
      {
        Serial.print("High Voltage Isolation Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag3[4]==1)
      {
        Serial.print("Redundant Power Supply Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag3[3]==1)
      {
        Serial.print("External Communication Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag3[2]==1)
      {
        Serial.print("Thermistor Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag3[1]==1)
      {
        Serial.print("Fan Monitor Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
      if(flag3[0]==1)
      {
        Serial.print("Weak Pack Fault");
        Serial.print(":");
        bms_fault_check += 1;
      }
    }

    if (bms_fault_check == 0){
      Serial.print("NO BMS FAULT");
      Serial.print(":");
    }
    Serial.print("@");
    Serial.println();
    
    //giving delay
    delay(100);
  }
}