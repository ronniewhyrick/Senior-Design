#include <SoftwareSerial.h>
SoftwareSerial GPSModule(10, 11); // RX, TX
SoftwareSerial HC12(6, 5); // HC-12 TX Pin, HC-12 RX Pin
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
bool txdata = false;
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
void setup() {
  Serial.begin(9600);
  GPSModule.begin(9600);
  HC12.begin(9600);   
  Serial.println("Setup Complete");
  HC12.write("Hello");
}

void loop() {
  Serial.flush();
  GPSModule.listen();
  GPSModule.flush();
  while (GPSModule.available() > 0)
  {
    //Serial.print("eyo");
    GPSModule.read();

  }
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
    }
    updates++;
    HC12.listen();
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    latfinal = nmea[2];
    Hlatfinal = nmea[3];
    lonfinal = nmea[4];
    Hlonfinal = nmea[5]; 
    latfinal.toCharArray(tx_lat,20);
    Hlatfinal.toCharArray(tx_Hlat,20);
    lonfinal.toCharArray(tx_lon,20);
    Hlonfinal.toCharArray(tx_Hlon,20);
    for (int i = 2; i < 6; i++) {
      Serial.print(labels[i]);
      Serial.print(nmea[i]);
      Serial.println("");
    }
    HC12.write(tx_lat);
    HC12.write(",");
    HC12.write(tx_Hlat);
    HC12.write(",");
    HC12.write(tx_lon);
    HC12.write(",");
    HC12.write(tx_Hlon);
    HC12.write('\n');
  }
  else {

    failedUpdates++;

  }
  stringplace = 0;
  pos = 0;
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
