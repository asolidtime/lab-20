/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/home/maxwell/CTD-IoT/d9/lab20-fsm/src/lab20-fsm.ino"
/*
 * Project lab20-fsm
 * Description:
 * Author:
 * Date:
 */
#include "MQTT.h"
void callback(char* topic, byte* payload, unsigned int length);
void exchangeStates();
void setup();
void loop();
#line 8 "/home/maxwell/CTD-IoT/d9/lab20-fsm/src/lab20-fsm.ino"
#define RED 0
#define YELLOW 1
#define GREEN 2
unsigned long long timer = 0;
bool northCarPresent = false;
bool eastCarPresent = false;
int northLightState = GREEN;
int eastLightState = RED;

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  if (String(topic).endsWith("north-car-sensor")) {
    if (String(p).equals("true")) {
      northCarPresent = true;
    } else {
      northCarPresent = false;
    }
  } else if (String(topic).endsWith("east-car-sensor")) {
    if (String(p).equals("true")) {
      eastCarPresent = true;
    } else {
      eastCarPresent = false;
    }
  }
  // write my code here
}
MQTT client("lab.thewcl.com", 1883, callback);


void exchangeStates() {
  client.publish("maximus-fsm/north-light-state", String(northLightState));
  client.publish("maximus-fsm/east-light-state", String(eastLightState));
}
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin func tions here.
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  if (client.isConnected()) {
    client.loop();
    switch (northLightState) {
      case GREEN:
        if (eastCarPresent) {
          northLightState = YELLOW;
          Serial.println("changing north light state to yellow");
          timer = millis();
        }
        break;
      case YELLOW:
        if (millis() - timer > 5000) {
          northLightState = RED;
          eastLightState = GREEN;
          timer = millis();
        }
        
        break;
      case RED:
        if (northCarPresent && !eastCarPresent && eastLightState != YELLOW) {
          eastLightState = YELLOW;
          timer = millis();
        }
        if (millis() - timer > 5000 && eastLightState == YELLOW) {
            eastLightState = RED;
            northLightState = GREEN;
        }
        break;
        
    }
    exchangeStates();

  } else {
    client.connect(System.deviceID());
    client.subscribe("maximus-fsm/north-car-sensor");
    client.subscribe("maximus-fsm/east-car-sensor");
    exchangeStates();
  }
}