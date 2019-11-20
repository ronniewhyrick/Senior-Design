#include <SoftwareSerial.h>
#define abs(x) ((x)>0?(x):-(x))
//establish two software serial ports
SoftwareSerial GPSModule(5, 6); // RX, TX
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
char rx_init[50];
bool done = false; // Listen check complete (0 = false, 1 = true)

void setup() {
  Serial.begin(9600);
  //initialize software serial for GPS then Trakker
  Trakker.begin(9600);
  GPSModule.begin(9600);
  Serial.println("Setup Complete");
}

void loop() {     
  Serial.flush();
  float T_LonInt;
  float T_LatInt;
  float B_LonInt;
  float B_LatInt; 
  float DeltaLon;
  float DeltaLat;
  float distance;
  //Begin reading GPS module
  while ((GPSModule.available()))
  {
    //Flush GPS buffer and begin reading data
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
      
      //Convert nmea longitude and latitude to readable degrees
      nmea[2] = ConvertLat();
      nmea[4] = ConvertLng();
      Serial.print("filtered self GPS coordinates \n");
      //Convert string arrays to strings, then character arrays
      latfinal = nmea[2];
      Hlatfinal = nmea[3];
      lonfinal = nmea[4];
      Hlonfinal = nmea[5]; 
      latfinal.toCharArray(tx_lat,20);
      Hlatfinal.toCharArray(tx_Hlat,20);
      lonfinal.toCharArray(tx_lon,20);
      Hlonfinal.toCharArray(tx_Hlon,20);
      
      //Print final GPS values with labels
      for (int i = 2; i < 6; i++) {
        Serial.print(labels[i]);
        Serial.print(nmea[i]);
        Serial.println("");
      }
    }
    else failedUpdates++;
    
    // Reset values and jump out of GPS while loop
    stringplace = 0;
    pos = 0;
    
    Trakker.listen(); //Priority set to trakker clips
  }

  //Begin reading Trakker clips
  while (Trakker.available())
  {
    String rx_init = Trakker.readStringUntil('\n');
//    Serial.print("rx_init: ");
//    Serial.println(rx_init);
    String T_Longitude = rx_init.substring(0,rx_init.indexOf(",")); //prints longitude substring
    T_LonInt = T_Longitude.toFloat();
    int del1 = T_Longitude.length();
    String T_Latitude = rx_init.substring(rx_init.indexOf(",",del1 + 2) + 1,rx_init.length() - 2); //prints longitude substring //might  have to be -4 at the end for \n attatched
    T_LatInt = T_Latitude.toFloat();
    Serial.println(T_LonInt,8);
    Serial.println(T_LatInt,8);
     //normalized coordinate system. 
     // 1m / 0.00001 degree change
     
     //Example Coordinates
     B_LonInt = 39.190953;
     B_LatInt = -096.584159;
     Serial.println(abs(B_LonInt),8);
//     Serial.println(B_LonInt,8);
//     Serial.println(B_LatInt,8);
     //find magnitude of distance between tracker and backpack
     DeltaLon = abs(B_LonInt) - abs(T_LonInt);
     Serial.println(DeltaLon,8);
     DeltaLat = abs(B_LatInt) - abs(T_LatInt);
     Serial.println(DeltaLat,8);
     distance = sqrt((DeltaLat*DeltaLat) + (DeltaLon*DeltaLon));
     Serial.print("Distance is: ");
     Serial.println(distance,8);
    //GPSModule.listen(); // Priority set to GPS
  }
  
 
}//end of void loop()


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
