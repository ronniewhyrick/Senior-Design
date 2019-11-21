/*
  Palpack.h - Library for Palpack backpack.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Palpack_h
#define Palpack_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class Palpack
{
  public:
    double DeltaLon1;
	double DeltaLat1;
	double degree;
	//Morse(int pin);
    //void dot();
    //void dash();
  private:
    //int _pin;
};

#endif