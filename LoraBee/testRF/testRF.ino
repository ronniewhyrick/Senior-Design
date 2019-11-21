#include <SoftwareSerial.h>
SoftwareSerial HC12(6, 5); // HC-12 TX Pin, HC-12 RX Pin
String TrakkerName = "Tx1"; //Name of this Clip
String rxData;         //will be compared to TrakkerName to see if transmit is needed
String CurrentLocation = "39.188239,N,-096.582849,W\n"; //location of chip will be updated consistantly

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12

}
void loop() {
  while (HC12.available()) {        // If HC-12 has data
  rxData = HC12.readStringUntil('\n');      // Read Transmitted data
  }
  if (rxData == "Tx1") {      // Check Transmitted data to see if chip is being called
    HC12.write("- 39.193602,N,-096.585778,W\n");      // Send that data to HC-12
    rxData = "0";
    delay(100);
  }
  if (rxData == "Tx2") {
    HC12.write("39.190983,N,-096.580575,W\n");      // Send that data to HC-12
    rxData = "0";
    delay(100);
  }
  if (rxData == "Tx3") {
    HC12.write("- 39.187612,N,-096.583708,W\n");      // Send that data to HC-12
    rxData = "0";
    delay(100);
  }
}
