#include <SoftwareSerial.h>
#include <math.h>
#define pi 3.14159265358979323846
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
char rxcharlat[20];
char rxcharlon[20];
String timeUp;
String nmea[15];
String latfinal;
String lonfinal;
String Hlatfinal;
String Hlonfinal;
float theta,dps,phi,phim;
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};
//backpack unique variables
String rx_lat;
String rx_lon;
bool done = false; // Listen check complete (0 = false, 1 = true)
bool rxd = false;
bool rxlat = false;
double latT;
double lonT;
double latB;
double lonB;

float deg2rad(float);
float rad2deg(float);

  
void setup() {
  Serial.begin(9600);
  Trakker.begin(9600);
  GPSModule.begin(9600);
}
void loop() {     
  Serial.flush();
  int lonlen;
  int latlen;
  float T_LonInt;
  float T_LatInt;
  float B_LonInt;
  float B_LatInt; 
  float DeltaLon;
  float DeltaLat;
  double DeltaLon1;
  double DeltaLat1;
  float distancetx;
  double ratio;
  double degreetx;


  //Begin reading GPS module
  delay (1000);
  while ((GPSModule.available()))
  {
   Serial.println("entered gps");
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
    }
    else failedUpdates++;
    
    // Reset values and jump out of GPS while loop
    stringplace = 0;
    pos = 0;
    Serial.println("Trakker Listen");
    Trakker.listen(); //Priority set to trakker clips
    
  }

      
 // Serial.println("JO");
  //Begin reading Trakker clips
  for (int j = 0; j <= 3; j++)
  {
    while (j == 0 && rxd == false)
    {
     Trakker.write("Tx1");
    Serial.println("Call Trakker 1");
    Serial.flush();
    delay(2000);
    if (Trakker.available())
    {
     Trakker.readString();
     Serial.println("Trakker 1 Link - Good");
     Serial.flush();
     rxd = true;
    }
    }
    while (j == 1 && rxd == false)
    {
     Trakker.write("Tx2");
     Serial.println("Call Trakker 2");
     Serial.flush();
     delay(2000);
     if (Trakker.available())
      {
       Trakker.readString();
       Serial.println("Trakker 2 Responded back");
       Serial.flush();
       rxd = true;
      }
    }
    while (j = 2 && rxd == false)
    { 
    Trakker.write("Tx3");
    Serial.println("Call Trakker 3");
    Serial.flush();
    delay(2000);
    if (Trakker.available())
    {
     Trakker.readString();
     Serial.println("Trakker 3 Responded back");
     Serial.flush();
     rxd = true;
     j = -1;
    }
    }
  while(!Trakker.available())
  {
    
  }

    while(Trakker.available())
    {
    rx_lat = Trakker.readString();
    latlen = rx_lat.length();
    Trakker.flush();
    Trakker.write("Lat Good");
    Serial.println("rx_in: ");
    while (!Trakker.available())
    {
      delay(10);
    }
    if (Trakker.available())
    {
    rx_lon = Trakker.readString();
    lonlen = rx_lon.length(); 
    }
    Serial.println(rx_lon);
    Serial.flush();
    Serial.println(rx_lat);
    Serial.flush();
    T_LonInt = rx_lon.toFloat();
    Serial.println(T_LonInt,5);
    T_LatInt = rx_lat.toFloat();
    Serial.print("Lat as Float");
    Serial.println(T_LatInt,5);
     
     //Example Coordinates
     B_LonInt = -096.584159;
     B_LatInt = 39.190953;
     //find magnitude of distance between tracker and backpack
     DeltaLon = abs(B_LonInt) - abs(T_LonInt);
     DeltaLat = abs(B_LatInt) - abs(T_LatInt);
    if (T_LonInt > B_LonInt) // Tracker is east of Backpack
    {
      if (T_LatInt > B_LatInt) //Tracker is North-east of Backpack
      {
       //First Quadrant
       // x & y are positive
     DeltaLon1 =  100000 * abs(DeltaLon);
     DeltaLat1 =  100000 * abs(DeltaLat);
      }
      else //Tracker is south-East of Backpack
      {
     DeltaLon1 = 100000 * abs(DeltaLon);
     DeltaLat1 = -1 * 100000 * abs(DeltaLat);
      }
    }
    else //Tracker is west of Backpack
    {
      if (T_LatInt > B_LatInt) //Tracker is North-west of Backpack
      {
      //Fourth Quadrant
      //x positive, y negative
     DeltaLon1 = -1 * 100000 * abs(DeltaLon);
     DeltaLat1 = 100000 * abs(DeltaLat);
      }
      else //Tracker is South-West of Backpack
      {
      //Third Quadrant 
      // both negative
     DeltaLon1 = -1 * 100000 * abs(DeltaLon);    //   <--------
     DeltaLat1 = -1 * 100000 * abs(DeltaLat);    //   <--------
      }
  }
  //   Serial.println(DeltaLat1);
//Serial.println(DeltaLon1);
     ratio = (DeltaLon1)/(DeltaLat1);
    // Serial.println(ratio); 
    degreetx = (180 * atan(ratio))/3.14159265359;  //   <--------
  //  Serial.println(degree); 
    distancetx = distance(B_LatInt,B_LonInt,T_LatInt,T_LonInt);
    Serial.print("distance in Kilometers : ");
    Serial.println(distancetx);
    Serial.print("angle from 0 : ");
    Serial.println(degreetx);
  }
  rxd = false;
  //Serial.println("Left while");
  }
  //Serial.println("Left For");
  GPSModule.listen();

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
      lngsecond = nmea[4].substring(i - 2).toFloat();
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

float distance(float lat1, float lon1, float lat2, float lon2) {
  float dist;
  
  if ((lat1 == lat2) && (lon1 == lon2)) {
    Serial.println("BAD");
    return 0;
  }
  else {
    theta = abs(abs(lon1) - abs(lon2));
    phi = abs(abs(lat1) - abs(lat2));
    phim = (abs(lat1) + abs(lat2)) / 2;
    theta = deg2rad(theta);
    phi = deg2rad(phi);
    phim = deg2rad(phim);
    float theta2 = theta * theta;
    float phi2 = phi * phi;
    dist = 6371.009  * sqrtf(phi2 + cosf(phim)*theta2);

    return (dist);
  }
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
float deg2rad(float deg) {
  float angle1 = deg * pi / 180;
  return (angle1);
  
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
float rad2deg(float rad) {
  return (rad * 180 / pi);
}
