#include <IRremote.h>

const unsigned int LEFT_INDICATOR_SIGNAL = 0x8F1B3EA4;
const unsigned int RIGHT_INDICATOR_SIGNAL = 0x53DEFA84;
const unsigned int ALL_INDICATORS_SIGNAL = 0x5CA55E68;
const unsigned int DAYS_LIGHTS_SIGNAL = 0x88748A00;
const unsigned int STOP_LIGHTS_SIGNAL = 0x5577900; // Not implemented

const unsigned int FRONT_LEFT_INDICATOR_PIN = 4;
const unsigned int FRONT_RIGHT_INDICATOR_PIN = 7;
const unsigned int DAYS_LIGHTS_INDICATOR_PIN[] = {8, 12};
const unsigned int REAR_LEFT_INDICATOR_PIN[] = {3, 5, 6}; // 3, 5, 6 - PWM pins
const unsigned int REAR_RIGHT_INDICATOR_PIN[] = {9, 10, 11}; // 9, 10, 11 - PWM pins

const unsigned int INDICATORS_DELAY = 600;
const int IR_PIN = 2;

IRrecv irrecv(IR_PIN);
decode_results signals;
unsigned long mainIndicatorsTimer = 0;
unsigned long rearIndicatorsTimer = 0;
unsigned int previousSignal = 0;
byte ledStates[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool leftIndicatorState = false; 
bool rightIndicatorState = false;
bool dayLightsState = false;
bool indicatorsToggleTrigger = false;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  pinMode(FRONT_LEFT_INDICATOR_PIN, OUTPUT);
  pinMode(FRONT_RIGHT_INDICATOR_PIN, OUTPUT);

  for (byte i = 0; i < sizeof DAYS_LIGHTS_INDICATOR_PIN; i++) {
    pinMode(DAYS_LIGHTS_INDICATOR_PIN[i], OUTPUT);
  }

  for (byte i = 0; i < sizeof REAR_LEFT_INDICATOR_PIN; i++) {
    pinMode(REAR_LEFT_INDICATOR_PIN[i], OUTPUT);
  }

  for (byte i = 0; i < sizeof REAR_RIGHT_INDICATOR_PIN; i++) {
    pinMode(REAR_RIGHT_INDICATOR_PIN[i], OUTPUT);
  }
}

void loop() {
  unsigned int signal = getIRSignal();

  if (signal == ALL_INDICATORS_SIGNAL) {
    if (signal == previousSignal) {
      leftIndicatorState = !leftIndicatorState;
      rightIndicatorState = leftIndicatorState;
    } else {
      // Reset the 500ms indicators time cycle
      mainIndicatorsTimer = millis();
      indicatorsToggleTrigger = true;
      leftIndicatorState = true;
      rightIndicatorState = true;
    }
    
  } else if (signal == LEFT_INDICATOR_SIGNAL) {
    if (signal == previousSignal) {
      leftIndicatorState = !leftIndicatorState;
    } else {
      // Reset the 500ms indicators time cycle
      mainIndicatorsTimer = millis();
      indicatorsToggleTrigger = true;
      leftIndicatorState = true;
    }
    
    rightIndicatorState = false;
    
  } else if (signal == RIGHT_INDICATOR_SIGNAL) {
    if (signal == previousSignal) {
      rightIndicatorState = !rightIndicatorState;
    } else {
      // Reset the 500ms indicators time cycle
      mainIndicatorsTimer = millis();
      indicatorsToggleTrigger = true;
      rightIndicatorState = true;
    }
    
    leftIndicatorState = false;
    
  } else if (signal == DAYS_LIGHTS_SIGNAL) {
    if (signal == previousSignal) {
      dayLightsState = !dayLightsState;
    } else {
      dayLightsState = true;
    }
  }

  if (leftIndicatorState) {
    enableLeftIndicator();
  } else {
    disableLeftIndicator();
  }
  
  if (rightIndicatorState) {
    enableRightIndicator();
  } else {
    disableRightIndicator();
  }

  if (dayLightsState) {
    enableDaysLight();
  } else {
    disableDaysLight();
  }

  unsigned long now = millis();

  if (!indicatorsToggleTrigger && now - mainIndicatorsTimer >= INDICATORS_DELAY) {
    indicatorsToggleTrigger = true;
    
    mainIndicatorsTimer = now;
  }
  
  if (indicatorsToggleTrigger && now - mainIndicatorsTimer >= INDICATORS_DELAY) {
    indicatorsToggleTrigger = false;

    mainIndicatorsTimer = now;
  }

  if (signal != 0) {
    previousSignal = signal;
  }

  for(int i = 0; i < sizeof ledStates; i++) {
    // Ignore 0 (RX), 1 (TX) pins and 2 (IR) pins
    if (i >= 3) {
      digitalWrite(i, ledStates[i]);
    }
  }
}

/**
 * Turn on left indicators (front and rear lights)
 */
void enableLeftIndicator() {
  setLedState(FRONT_LEFT_INDICATOR_PIN, indicatorsToggleTrigger ? 1 : 0);

  // Different animation for emergency button
  if (leftIndicatorState && rightIndicatorState) {
    for (byte i = 0; i < sizeof REAR_LEFT_INDICATOR_PIN; i++) {
      setLedState(REAR_LEFT_INDICATOR_PIN[i], indicatorsToggleTrigger ? 1 : 0);
    }
  } else {
    // Reset all rear lights
    for (byte i = 0; i < sizeof REAR_LEFT_INDICATOR_PIN; i++) {
      setLedState(REAR_LEFT_INDICATOR_PIN[i], 0);
    }
    
    // Left indicator animation
    if (indicatorsToggleTrigger) {
      unsigned long now = millis();
  
      if (rearIndicatorsTimer == 0) {
        rearIndicatorsTimer = now;
      }
  
      setLedState(REAR_LEFT_INDICATOR_PIN[0], 1);
  
      if (now - rearIndicatorsTimer >= INDICATORS_DELAY / 3) {
        setLedState(REAR_LEFT_INDICATOR_PIN[1], 1);
      }
      
      if (now - rearIndicatorsTimer >= INDICATORS_DELAY / 1.5) {
        setLedState(REAR_LEFT_INDICATOR_PIN[2], 1);
      }
    } else {
      rearIndicatorsTimer = 0;
    }
  }
}

/**
 * Turn off left indicators (front and rear lights)
 */
void disableLeftIndicator() {
  setLedState(FRONT_LEFT_INDICATOR_PIN, 0);

  for (byte i = 0; i < sizeof REAR_LEFT_INDICATOR_PIN; i++) {
    setLedState(REAR_LEFT_INDICATOR_PIN[i], 0);
  }
}

/**
 * Turn on right indicators (front and rear lights)
 */
void enableRightIndicator() {
  setLedState(FRONT_RIGHT_INDICATOR_PIN, indicatorsToggleTrigger ? 1 : 0);

  // Different animation for emergency button
  if (leftIndicatorState && rightIndicatorState) {
    for (byte i = 0; i < sizeof REAR_RIGHT_INDICATOR_PIN; i++) {
      setLedState(REAR_RIGHT_INDICATOR_PIN[i], indicatorsToggleTrigger ? 1 : 0);
    }
  } else {
    // Reset all rear lights
    for (byte i = 0; i < sizeof REAR_RIGHT_INDICATOR_PIN; i++) {
      setLedState(REAR_RIGHT_INDICATOR_PIN[i], 0);
    }
      
    // Right indicator animation
    if (indicatorsToggleTrigger) {
      unsigned long now = millis();
  
      if (rearIndicatorsTimer == 0) {
        rearIndicatorsTimer = now;
      }
      
      setLedState(REAR_RIGHT_INDICATOR_PIN[0], 1);
  
      if (now - rearIndicatorsTimer >= INDICATORS_DELAY / 3) {
        setLedState(REAR_RIGHT_INDICATOR_PIN[1], 1);
      }
      
      if (now - rearIndicatorsTimer >= INDICATORS_DELAY / 1.5) {
        setLedState(REAR_RIGHT_INDICATOR_PIN[2], 1);
      }
    } else {
      rearIndicatorsTimer = 0;
    }  
  }
}

/**
 * Turn off right indicators (front and rear lights)
 */
void disableRightIndicator() {
  setLedState(FRONT_RIGHT_INDICATOR_PIN, 0);

  for (byte i = 0; i < sizeof REAR_RIGHT_INDICATOR_PIN; i++) {
    setLedState(REAR_RIGHT_INDICATOR_PIN[i], 0);
  }
}

/**
 * Turn on days light (front and rear lights)
 */
void enableDaysLight() {
  for (byte i = 0; i < sizeof DAYS_LIGHTS_INDICATOR_PIN; i++) {
    setLedState(DAYS_LIGHTS_INDICATOR_PIN[i], 1);
  }

  if (!leftIndicatorState) {
    for (byte i = 0; i < sizeof REAR_LEFT_INDICATOR_PIN; i++) {
      setLedState(REAR_LEFT_INDICATOR_PIN[i], 1);
    }
  }

  if (!rightIndicatorState) {
    for (byte i = 0; i < sizeof REAR_RIGHT_INDICATOR_PIN; i++) {
      setLedState(REAR_RIGHT_INDICATOR_PIN[i], 1);
    } 
  }
}

/**
 * Turn off days light (front and rear lights)
 */
void disableDaysLight() {
  for (byte i = 0; i < sizeof DAYS_LIGHTS_INDICATOR_PIN; i++) {
    setLedState(DAYS_LIGHTS_INDICATOR_PIN[i], 0);
  }

  if (!leftIndicatorState) {
    for (byte i = 0; i < sizeof REAR_LEFT_INDICATOR_PIN; i++) {
      setLedState(REAR_LEFT_INDICATOR_PIN[i], 0);
    }
  }

  if (!rightIndicatorState) {
    for (byte i = 0; i < sizeof REAR_RIGHT_INDICATOR_PIN; i++) {
      setLedState(REAR_RIGHT_INDICATOR_PIN[i], 0);
    }
  }
}

void setLedState(unsigned int pin, byte mode) {
  ledStates[pin] = mode;
}

/**
 * Retrive IR signal code
 */
double getIRSignal() {
  unsigned int signal = 0;
  
  if (irrecv.decode(&signals)) {
    //Serial.println(signals.decode_type);
    Serial.print(F("IR code = 0x"));
    Serial.println(signals.value, HEX);
    //Serial.println(signals.bits);
    irrecv.blink13(false);

    signal = signals.value;
    
    irrecv.resume();  
  }

  return signal;
}
