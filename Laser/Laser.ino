/* Laser controller */
/* Mark J Cox mark@awe.com September 2013 */

#define pinMsg7reset 3
#define pinMsg7strobe 2
#define pinMsg7out A0
#define pinMotor 10
#define pinTTL 11

#include <ooPinChangeInt.h>
#include <AdaEncoder.h>
#include <RemoteSwitch.h>
#include "pushbuttonswitch.h"

AdaEncoder encoder = AdaEncoder('a', 8, 9);
pushbuttonswitch switcha = pushbuttonswitch(4,"a");

void setup() {
  pinMode(pinMotor, OUTPUT);
  pinMode(pinTTL, OUTPUT);
  pinMode(pinMsg7reset, OUTPUT);
  pinMode(pinMsg7strobe, OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  pinMode(pinMsg7out, INPUT);
  digitalWrite(pinMsg7out, LOW); // No pullup
  Serial.begin(9600);
}

int8_t oldclicks;
int mode = 0;
int lastmode = -1;
int speed = 255;
int toggle = 0;
int strobespeed = 1000;
int pot;

void loop() {
  int changedmode = 0;

  if (switcha.getCount()>0) {
    mode++;
    Serial.print("Change to mode ");
    Serial.println(mode);
    delay(500);
    flashmode(mode);
    switcha.reset();
  }
  if (mode == 0) {
    // off 
    if (mode != lastmode) {
      Serial.println("mode=off");
      digitalWrite(pinTTL,0);
      analogWrite(pinMotor, 0);
      lastmode = mode;
    }     
  } else
  if (mode == 1) {
    // Constantly on, controller is speed
    if (mode != lastmode) {
      Serial.println("mode=on, change speed with control");
      digitalWrite(pinTTL,1);
      analogWrite(pinMotor,speed);
      lastmode = mode;
    }       
    int8_t newclicks = encoder.getClicks();
    int8_t clicks = newclicks - oldclicks;
    if (clicks != 0) {
      speed += (clicks*8);
      if (speed>255) { 
        speed = 255; 
      }
      if (speed<80) { 
        speed = 80; 
      }
      Serial.print("new speed is ");
      Serial.println(speed);
      oldclicks = newclicks;
      if (speed > 80) {
        analogWrite(pinMotor,speed); 
      } 
      else {
        analogWrite(pinMotor,0);
      }
    }

  } 
  else if (mode ==2) {
    // strobe on, controller is speed of strobe
    if (mode != lastmode) {
      Serial.println("mode=strobe, change strobe speed with control");
      strobespeed = 500;
      toggle = 1;
      lastmode = mode;
    }       
    int8_t newclicks = encoder.getClicks();
    int8_t clicks = newclicks - oldclicks;
    if (clicks != 0) {
      strobespeed += (clicks*50);
      if (strobespeed<0) { 
        strobespeed = 0; 
      }
      if (strobespeed == 0) {
        toggle = 1;
        digitalWrite(pinTTL,1);
      }
      Serial.print("new strobe speed is ");
      Serial.println(strobespeed);
      oldclicks = newclicks;
    }
    digitalWrite(pinTTL,toggle);
    if (strobespeed >0) {
      toggle++;  
      if (toggle > 1) { 
        toggle = 0; 
      }
      delay(strobespeed);
    }
  } 
  else if (mode ==3) {
    if (mode != lastmode) {
      lastmode = mode;
      resetmsgeq7();
      Serial.println("mode=voice");
      digitalWrite(pinTTL,1);

      pot=20;
    }
    int8_t newclicks = encoder.getClicks();
    int8_t clicks = newclicks - oldclicks;
    if (clicks != 0) {
      pot += (clicks*1);
      if (pot<0) {pot = 0;}
      if (pot>1024) {pot = 1024;}
      oldclicks = newclicks;
      Serial.print("pot ");
      Serial.println(pot);
    }
    int bass = msgeq7(100);
    long x = (bass * pot);
    x = x/20;
    if (x > 1024) { x = 1024;}
    
    int PWMvalue = map((int)x, 0, 1024, 80, 255);  // scale analogRead's value to Write's 255 max
    if (PWMvalue < 81) { 
          PWMvalue = 0; 
    }        
    analogWrite(pinMotor, PWMvalue);
//    Serial.print(bass,DEC);
//    Serial.println("");
  }
  
  
    else if (mode ==4) {
    if (mode != lastmode) {
      lastmode = mode;
      resetmsgeq7();
      Serial.println("mode=voice2");
      digitalWrite(pinTTL,1);

      pot=500;
    }
    int8_t newclicks = encoder.getClicks();
    int8_t clicks = newclicks - oldclicks;
    if (clicks != 0) {
      pot += (clicks*20);
      if (pot<0) {pot = 0;}
      if (pot>1024) {pot = 1024;}
      oldclicks = newclicks;
      Serial.print("pot ");
      Serial.println(pot);
    }
    int bass = msgeq7(pot);
    int PWMvalue = map(bass, 0, 1024-pot, 80, 255);  // scale analogRead's value to Write's 255 max
    if (PWMvalue < 81) { 
          PWMvalue = 0; 
    }        
    analogWrite(pinMotor, PWMvalue);
//    Serial.print(bass,DEC);
//    Serial.println("");
    }
  
  
  
    else if (mode ==5) {
    if (mode != lastmode) {
      lastmode = mode;
      resetmsgeq7();
      pot = 500;
      Serial.println("mode=voicestrobe");
            if (speed > 80) {
        analogWrite(pinMotor,speed); 
      } 
      else {
        analogWrite(pinMotor,0);
      }
    }
    
        int8_t newclicks = encoder.getClicks();
    int8_t clicks = newclicks - oldclicks;
    if (clicks != 0) {
      pot += (clicks*10);
      if (pot<0) {pot = 0;}
      oldclicks = newclicks;
    }
    
    int bass = msgeq7(100);

    if (bass > pot) {
      digitalWrite(pinTTL,1);
    } else {
      digitalWrite(pinTTL,0);
    }      
    }
      else if (mode ==6) {
    if (mode != lastmode) {
      lastmode = mode;
      resetmsgeq7();
      pot = 500;
            digitalWrite(pinTTL,1);

      Serial.println("mode=voicestrobe2");
    }
    
        int8_t newclicks = encoder.getClicks();
    int8_t clicks = newclicks - oldclicks;
    if (clicks != 0) {
      pot += (clicks*10);
      if (pot<0) {pot = 0;}
      oldclicks = newclicks;
    }
    
    int bass = msgeq7(100);

    if (bass > pot) {
        analogWrite(pinMotor,255); 
      } 
      else {
        analogWrite(pinMotor,0);
      }   
      
 delay(50);
} 
  else {
    Serial.println("setting mode back to 0");
    mode = 0;
  }
}

void resetmsgeq7() {
    digitalWrite(pinMsg7reset, HIGH);			// reset the MSGEQ7's counter
    digitalWrite(pinMsg7strobe, HIGH);	
    delayMicroseconds(35);
    digitalWrite(pinMsg7reset, LOW);  
}

void flashmode(int mode) {
  for (int i=0; i< mode; i++) {
      digitalWrite(13,1);
      delay(130);
      digitalWrite(13,0);
      delay(130);
    }
  
}

int msgeq7(int pot) {
    resetmsgeq7();
    int bass = 0;

    for (int x = 0; x < 1; x++){
      digitalWrite(pinMsg7strobe, LOW);		// output each DC value for each freq band
      delayMicroseconds(40); // to allow the output to settle
      int spectrumRead = analogRead(0);

      // int pot = 100;
      // there is a noise floor, let's call it 100

      if (x==0 ) {
          spectrumRead -= pot;
          if (spectrumRead < 0) spectrumRead = 0;
          bass = spectrumRead;
      }
      digitalWrite(pinMsg7strobe, HIGH);
    }
    delay(20); // 50 is good too
    return bass;
}
