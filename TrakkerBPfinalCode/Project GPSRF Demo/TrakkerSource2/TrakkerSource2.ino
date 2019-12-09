#include <SoftwareSerial.h>
SoftwareSerial GPSModule(11, 10); // RX, TX
SoftwareSerial HC12(6, 5); // HC-12 TX Pin, HC-12 RX Pin
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
bool found = false;
bool enter1 = true;
char tx_lat[20];
char tx_lon[20];
char tx_Hlat[20];
char tx_Hlon[20];
//char tx_coordinates[25];
String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};
String txinput;
String bpCallID;
int m = 0;
void setup() {
  Serial.begin(9600);
  GPSModule.begin(9600);
  HC12.begin(9600);   
  Serial.println("Setup Complete");
}

void loop() {
  GPSModule.listen();
  while (GPSModule.available() > 0)
  {
    //Serial.print("eyo");
    GPSModule.read();
  }
  if (GPSModule.find("$GPRMC,")) {
    Serial.println("Grabbing GPS data");
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
    }
    updates++;
    Serial.println("HC12 Listening");
//    nmea[2] = ConvertLat();
//    nmea[4] = ConvertLng();
    HC12.listen();
    while(!HC12.available() && found == false)
    {
      //wait here till trakker call is received
      }
    while(HC12.available())
    {
       found = true;
       bpCallID = HC12.readString();
      // Serial.print("Call ID Rxd: ");
       if (bpCallID == "Tx2")
        {
       Serial.print("Call ID Rxd: ");
       Serial.println(bpCallID);
       Serial.flush();
       HC12.write("Rxd");
       Serial.println("Trakker 2 - Linked");
       Serial.println("Enter Trakker 2 Coordinates: ");
    while(!Serial.available())
    {
      
    }
    if (Serial.available())
    {
    txinput = Serial.readString();
    }
  //  Serial.println(txinput);
    Serial.flush();
    String T_Longitude = txinput.substring(0,txinput.indexOf(",")); //prints longitude substring
    String T_Latitude = txinput.substring(txinput.indexOf(",") + 1,txinput.length()); //prints longitude substring //might  have to be -4 at the end for \n attatched
    int LenLon = T_Longitude.length();
    int LenLat = T_Latitude.length();
//    Serial.println(T_Longitude);
//    Serial.println(T_Latitude);
//    for (int i = 2; i < 6; i++) {
//      Serial.print(labels[i]);
//      Serial.print(nmea[i]);
//      Serial.println("");
//    }
    //Serial.println(txinput);
    char txlon[LenLon];
    char txlat[LenLat];
    T_Longitude.toCharArray(txlon,LenLon);
    T_Latitude.toCharArray(txlat,LenLat);
    Serial.print("tx_coordinates: ");
    Serial.print(txlon);
    Serial.print(",");
    Serial.println(txlat);
    Serial.flush();
    HC12.write(txlon);
    HC12.flush();
    while (!HC12.available())
    {
      
    }
    while (HC12.available())
    {
      String Latgood = HC12.readString();
      if (Latgood == "Lat Good");
      {
        HC12.write(txlat);
      }
    }
    Serial.println("Trakker Comm - Complete");
     }
    else { Serial.println("No Connection made");}
    found = false; 
    }
  }
  else failedUpdates++;
  stringplace = 0;
  pos = 0;
  GPSModule.listen();
}

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
