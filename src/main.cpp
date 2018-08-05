#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "HX711.h"
#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>
/*
#
# Settings
#
*/
const int numRows = 2;	
const int numCols = 16;		

#define Pin_Btn1 6
#define Pin_Btn2 7
#define PIN_BUTTON_UP 8
#define Pin_RT1 A1
#define Pin_RT2 A2
#define Pin_LED 10

#define group1_debouncetime  4


HX711 scale(A3, A4);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/*
#
# Prototypes
#
*/
float getweight(int n);
void pciSetup(byte pin);
void pinSetup();



/*
#
# Global Vars
#
*/
unsigned int group1_lasttoggle = 0;
unsigned int RT1_lasttoggle = 0;
unsigned int RT2_lasttoggle = 0;
int RT_lastpos = 0;
int RT_currpos = 0;


void setup() {
pinSetup();
lcd.begin(numCols, numRows);
lcd.print("Welcome!");
scale.set_scale(0);                      // this value is obtained by calibrating the scale with known weights; see the README for details
delay(100);
scale.get_value(5);
scale.tare();				        // reset the scale to 0
delay(100);
scale.tare();		
}

void loop() {
lcd.clear();
lcd.print(RT_currpos);
delay(200);
}



void pinSetup(){
    pinMode(Pin_Btn1, INPUT);
	digitalWrite(Pin_Btn1, HIGH);
	pciSetup(Pin_Btn1);

    pinMode(PIN_BUTTON_UP, INPUT);
	digitalWrite(PIN_BUTTON_UP, HIGH);
	pciSetup(PIN_BUTTON_UP);

	pinMode(Pin_Btn2, INPUT);
	digitalWrite(Pin_Btn2, HIGH);
	pciSetup(Pin_Btn2);

	pinMode(Pin_RT1, INPUT);
	digitalWrite(Pin_RT1, HIGH);
	pciSetup(Pin_RT1);

	pinMode(Pin_RT2, INPUT);
	digitalWrite(Pin_RT2, HIGH);
	pciSetup(Pin_RT2);

	pinMode(Pin_LED, OUTPUT);

	RT_lastpos = digitalRead(Pin_RT1);
	digitalWrite(Pin_LED, LOW);
}

float getweight(int n){
  float floatlist[n];
  float max , min = 0;
  int maxpos, minpos = 0;
  float sum = 0;
  do {
    max = 0;
    min = 999999999;
    for ( int i = 0; i < n; i++){
    floatlist[i] = scale.get_units(1);
    }
  
    
    for ( int i = 0; i < n; i++){
      if (floatlist[i] > max){
        max = floatlist[i];
        maxpos = i;
      }
      if (floatlist[i] < min){
        min = floatlist[i];
        minpos = i;
      }
    }
  } while (((max - min) / min) > 0.1 && (min + max) > 5);
  


  for(int i = 0; i < n; i++){
    if (i != maxpos && i != minpos){
      sum += floatlist[i];
    }
  }
  sum = sum / ( n - 2);
  return sum;
}


ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
}


ISR (PCINT0_vect) // ISR for Pins 8 - 13
{

}

ISR (PCINT1_vect)
{
	if (((millis() - group1_lasttoggle) >= group1_debouncetime))
	{
		group1_lasttoggle = millis();

		if ((digitalRead(Pin_RT1)) != (digitalRead(Pin_RT2)))
		{
			if (RT_lastpos == 1)
			{
				if (digitalRead(Pin_RT1) > digitalRead(Pin_RT2))
				{
					//Step CCW
					RT_currpos++;
				}
				else if (digitalRead(Pin_RT1) < digitalRead(Pin_RT2))
				{
					//Step CW
					RT_currpos--;
				}
			}

			if (RT_lastpos == 0)
			{
				if (digitalRead(Pin_RT1) > digitalRead(Pin_RT2))
				{
					//Step CW
					RT_currpos--;
				}
				else if (digitalRead(Pin_RT1) < digitalRead(Pin_RT2))
				{
					//Step CCW
					RT_currpos++;
				}
			}
		}

		if ((digitalRead(Pin_RT1)) == (digitalRead(Pin_RT2)))
		{
			RT_lastpos = digitalRead(Pin_RT1);
		}

	}
}





void pciSetup(byte pin)
{
	*digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));  // enable pin
	PCIFR |= bit(digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit(digitalPinToPCICRbit(pin)); // enable interrupt for the group
}