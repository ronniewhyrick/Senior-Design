#include <SoftwareSerial.h>
SoftwareSerial HC12(6, 5); // HC-12 TX Pin, HC-12 RX Pin
String TrakkerName = "Tx1"; //Name of this Clip
String rxData;         //will be compared to TrakkerName to see if transmit is needed
String txData;
String LatS; //location of chip will be updated consistantly
String LonS;
float Lat = -096.582849;
float Lon = 39.188239;
char Send[20];
void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
}

void loop() {
  Lat += 0.00002;
  Lon -= 0.00001;
  LatS = String(Lat,6);
  LonS = String(Lon,6);
  while (HC12.available()) {      // If HC-12 has data
    rxData = HC12.readStringUntil('\n');      // Read Transmitted data
  }
  if (rxData == "Tx1") { 
    // Check Transmitted data to see if chip is being called

    txData = LatS+",N,"+LonS+",W\n";
    txData.toCharArray(Send,40);
    Serial.println(Send);
    HC12.write(Send);      // Send that data to HC-12
    rxData = "";
    LatS = "";
    LonS = "";
    memset(Send, 0, sizeof(Send));
    HC12.flush();
  }
}
