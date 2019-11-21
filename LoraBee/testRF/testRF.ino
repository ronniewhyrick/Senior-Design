#include <SoftwareSerial.h>
SoftwareSerial HC12(6, 5); // HC-12 TX Pin, HC-12 RX Pin
String TrakkerName = "Tx1"; //Name of this Clip
String rxData;         //will be compared to TrakkerName to see if transmit is needed
String txData;
String LatS; //location of chip will be updated consistantly
String LonS;
long Lat = -096.582849;
long Lon = 39.188239;
char Send[20];
void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12

}
long latl = 2304;
long lonl = 1023;
void loop() {

  while (HC12.available()) {      // If HC-12 has data
    rxData = HC12.readStringUntil('\n');      // Read Transmitted data
  }
  if (rxData == "Tx1") { 
    Serial.println("begin");
    // Check Transmitted data to see if chip is being called
    Lat += 0.00002;
   // Serial.println(Lat,6);
    Lon -= 0.00001;
    latl += 2;
    lonl -= 1;
    LatS = String(latl,6);
    LonS = String(lonl,6);
    Serial.println("increment");
    txData = LatS+",N,"+LonS+",W\n";
    txData.toCharArray(Send,40);
    Serial.println(Send);
    HC12.write(Send);      // Send that data to HC-12
    rxData = "";
    LatS = "";
    LonS = "";
    Serial.println("clear");
    memset(Send, 0, sizeof(Send));
        HC12.flush();
  }
}
