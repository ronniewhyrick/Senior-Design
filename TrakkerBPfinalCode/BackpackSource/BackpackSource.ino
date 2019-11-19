#include <SoftwareSerial.h>
//establish two software serial ports
SoftwareSerial GPSModule(5, 4); // RX, TX
SoftwareSerial Trakker(9, 10); // HC-12 TX Pin, HC-12 RX Pin
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
char tx_lat[20];
char tx_lon[20];
char tx_Hlat[20];
char tx_Hlon[20];
String timeUp;
String nmea[15];
String latfinal;
String lonfinal;
String Hlatfinal;
String Hlonfinal;
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};
//backpack unique variables
char rx_init;
bool done = false; // Listen check complete (0 = false, 1 = true)

void setup() {
  Serial.begin(9600);
  //initialize software serial for GPS then Trakker
  Trakker.begin(9600);
  GPSModule.begin(9600);
  Serial.println("Setup Complete");
}

void loop() {     //following code till end of loop is for testing w/o GPS only. allows data to be recieved without having a working GPS.
  Serial.flush();
  //Begin reading GPS module
  //GPSModule.listen(); // Priority set to GPS
  while ((GPSModule.available())) //if new GPS data is available and all other process complete
  {
    //activate software serial to GPS port, ignore Trakker port
    GPSModule.flush();
    GPSModule.read();
    
    //Begin taking in nmea coordinates
    if (GPSModule.find("$GPRMC,")) {
      String tempMsg = GPSModule.readStringUntil('\n');
      for (int i = 0; i < tempMsg.length(); i++) {
        if (tempMsg.substring(i, i + 1) == ",") {
          nmea[pos] = tempMsg.substring(stringplace, i);
          stringplace = i + 1;
          pos++;
        }
        if (i == tempMsg.length() - 1) {
          nmea[pos] = tempMsg.substring(stringplace, i);
        }
      }updates++;
    //call functions to convert nmea long and lat to readable degrees
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    Serial.print("filtered self GPS coordinates \n");
    //enable Trakker Software Serial port.
    //convert string arrays to strings, then character arrays
    latfinal = nmea[2];
    Hlatfinal = nmea[3];
    lonfinal = nmea[4];
    Hlonfinal = nmea[5]; 
    latfinal.toCharArray(tx_lat,20);
    Hlatfinal.toCharArray(tx_Hlat,20);
    lonfinal.toCharArray(tx_lon,20);
    Hlonfinal.toCharArray(tx_Hlon,20);
    //print on monitor final self GPS values with labels
      for (int i = 2; i < 6; i++) {
        Serial.print(labels[i]);
        Serial.print(nmea[i]);
        Serial.println("");
      }
     //Serial.println(Trakker.isListening());
    }
    else failedUpdates++;
    // Reset values and jump out of GPS while loop
    stringplace = 0;
    pos = 0;
    //Begin reading Trakker clips
    Trakker.listen(); //Priority set to trakker clips
  }
  
  while (Trakker.available())
  {
     rx_init = Trakker.read();
     Serial.println(Trakker.read());
     Serial.print(rx_init);
     GPSModule.listen(); // Priority set to GPS
  }
}//end of void loop()


//everything below are just functions to convert nmea data to readable degrees and hemispherical information
String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }
  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}
