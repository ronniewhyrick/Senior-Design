#include <SoftwareSerial.h>
#include <math.h>
#define pi 3.14159265358979323846
#define abs(x) ((x)>0?(x):-(x))
//establish two software serial ports
SoftwareSerial GPSModule(10, 9); // RX,TX
SoftwareSerial Trakker(6, 5); // HC-12 TX Pin, HC-12 RX Pin
SoftwareSerial lcd(2,3); //RX,TX

int updates,failedUpdates,pos;
int stringplace = 0;
char tx_lat[20];
char tx_lon[20];
char rxcharlat[20];
char rxcharlon[20];
String nmea[15];
float theta,dps,phi,phim;
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};
//backpack unique variables
String rx_lat,rx_lon;
bool rxd = false;
//Example Coordinates centered at KSU Engineering Building
float B_LonInt = -096.584159;
float B_LatInt = 39.190953;

float deg2rad(float);
float rad2deg(float);

  
void setup() {
  Serial.begin(9600);
  Serial.flush();
  Serial.println("Backpack Startup");
  lcd.begin(9600);
  Trakker.begin(9600);
  GPSModule.begin(9600);
}
void loop() {     
  int lonlen;
  int latlen;
  float T_LonInt;
  float T_LatInt;
  float distancetx,angle;
  float xtx,ytx;
  //Begin reading GPS module
  delay (5000);
  while ((GPSModule.available()))
  {
   //Serial.println("entered gps");
    //Flush GPS buffer and begin reading data
    GPSModule.flush();
    GPSModule.read();
  }
    //Begin taking in nmea coordinates
    if (GPSModule.find("$GPRMC,")) {
      Serial.println("Self GPS Finding");
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
      Serial.println("Self GPS Converting");
      nmea[2] = ConvertLat();
      nmea[4] = ConvertLng();
      //Convert string arrays to strings, then character arrays
      nmea[2].toCharArray(tx_lat,20);
      nmea[4].toCharArray(tx_lon,20);

//      char temp[50];
//      String temps = tx_lat + "," + tx_lon;
//      temps.toCharArray(
//      strcpy(temp, ", ");
//      strcpy(temp, tx_lat);
//      lcd.write(temp);  // <------------Reading in backpack location and print as current location
    }
    else failedUpdates++;
    
    // Reset values and jump out of GPS while loop
    stringplace = 0;
    pos = 0;
    Serial.println("Self GPS Found");
    Serial.println("Trakker Enable");
   Trakker.listen();
//Begin reading Trakker clips
  for (int j = 0; j < 4 ; j++)
  {
    j = 0;
    while (j == 0 && rxd == false)
    {
      Trakker.write("Tx1");
      Serial.println("Trakker1 - Calling");
      Serial.flush();
      delay(2000);
      if (Trakker.available())
      {
         Serial.println(Trakker.readString());
         Serial.println("Trakker1 - Responded ");
         Serial.flush();
         rxd = true;
      }
    }
    while (j == 1 && rxd == false)
    {
       Trakker.write("Tx2");
       Serial.println("Trakker2 - Calling");
       Serial.flush();
       delay(2000);
       if (Trakker.available())
        {
           Trakker.readString();
           Serial.println("Trakker2 - Responded");
           Serial.flush();
           rxd = true;
        }
    }
    while (j == 2 && rxd == false)
    { 
      Trakker.write("Tx3");
      Serial.println("Trakker3 - Calling");
      Serial.flush();
      delay(2000);
      if (Trakker.available())
      {
         Trakker.readString();
         Serial.println("Trakker3 - Responded");
         Serial.flush();
         rxd = true;
         j = -1;
      }
    }
  
  while(!Trakker.available()){
    /* delay input read in*/
    Serial.println("Trakker unavailable");
    delay(1000);
    }
  while(Trakker.available())
  {
    Serial.println("HERE");
    rx_lat = Trakker.readString();
    latlen = rx_lat.length();
    Trakker.flush();
    Trakker.write("Lat Good");
    while (!Trakker.available())
    {
      delay(10);
    }
    if (Trakker.available())
    {
      rx_lon = Trakker.readString();
      lonlen = rx_lon.length(); 
    }
    T_LonInt = rx_lon.toFloat();
    Serial.print("Trakker Longitude Rxd: ");
    Serial.println(T_LonInt,5);
    T_LatInt = rx_lat.toFloat();
    Serial.print("Trakker Latitude Rxd: ");
    Serial.println(T_LatInt,5);
     
   //find magnitude of distance between tracker and backpack
    distancetx = distance(B_LatInt,B_LonInt,T_LatInt,T_LonInt);
    float dy = T_LatInt - B_LatInt;
    float dx = cosf(pi/180*B_LatInt)*(T_LonInt - B_LonInt);
    angle =(180 * atan2f(dy, dx))/pi;
    Serial.print("Angle is : ");
    Serial.println(angle);
    Serial.print("distance in Kilometers : ");
    Serial.println(distancetx);

    if (0 < angle && angle < 90)
    {
      Serial.println("First Quadrant");
      xtx = distancetx * cosf(angle * pi/180);
      ytx = distancetx * sinf(angle *pi/180);
      
    }
    else if (angle > 90)
    {
      Serial.println("Second Quadrant");
      angle = 180 - angle;
      xtx = -1 * distancetx * cosf(angle * pi/180);
      ytx = distancetx * sinf(angle *pi/180);      
    }
    else
    {
      if (angle < -90)
      {
        Serial.print("Third Quadrant");
        angle = 180 - abs(angle);
        xtx = -1 * distancetx * cosf(angle * pi/180);
        ytx = -1 * distancetx * sinf(angle *pi/180);          
      }
      else
      {
        Serial.print("Fourth Quadrant");
        angle = abs(angle);
        xtx = distancetx * cosf(angle * pi/180);
        ytx = -1 * distancetx * sinf(angle *pi/180);          
      }
    }
    String sxtx = String(xtx);
    String sytx = String(ytx);
    String tx = String(B_LatInt) + "," + String(B_LonInt) + "," + sxtx + "," + sytx;
    int len = tx.length();
    char txfinal[len];
    tx.toCharArray(txfinal,len);
    Serial.println("TX to Nano");
    Serial.println(tx);
    Serial.println(xtx);
    Serial.println(ytx);
    lcd.write(txfinal);

   // lcd.write(ytx);
  }
  rxd = false;
  }
  GPSModule.listen();

}//end of void loop()

///////////////////////////////////////////////////////////////

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
