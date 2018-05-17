
/*
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D
 */

#include "Arduino.h"
#include "OneButton.h"
#include "math.h"

#define DEBUG 1
#define THERMISTORPIN A0
#define BUTTONPIN A1
#define BUZZERPIN A2  // A2
#define MOSFETPIN A3
#define ADCVOLT   A6

#define THERMISTORNOMINAL 216000  // resistance at 25 degrees C
#define TEMPERATURENOMINAL 25     // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 20             // how many samples to take and average, more takes longer
#define BCOEFFICIENT 4168.8       // The beta coefficient of the thermistor (usually 3000-4000)
#define SERIESRESISTOR 9000             // resistor value of voltage divider
#define INTERVAL 1000UL * 2             // interval at which to take a temp measurement
#define BUZZERINTERVAL 1000UL * 60 * 2  // interval at which a buzzer will be reset back to ON - 2 min
#define IDLEINTERVAL 1000UL * 60 * 5    // interval at which we'll keep the thermometer running without temp change

typedef enum { ON, OFF } Buzzer;                            // buzzer options

int samples[NUMSAMPLES];
unsigned long previousMillis = 0; // will store last time a temp measurement was updated
unsigned long buzzerMillis = 0;   // will store last time a buzzer was reset
unsigned long idleMillis = 0;   // will store last time a idle was reset
bool hasMilkWarmedUp = false;
OneButton button(BUTTONPIN, 1);   // button when pressed will take A1 to ground, active low
Buzzer buzzer = ON;               // Initial buzzer status; allow the buzzer to sound by default
float oldTemperatureValue = 0.0;        // keep track of previous temperature value
bool firstRun = true;

void toggleBuzzer_singlePress() {
    if (buzzer == ON) {
        buzzer = OFF;
        if (DEBUG) Serial.println("Buzzer: OFF");
    }
    else {
        buzzer = ON;
        if (DEBUG) Serial.println("Buzzer: ON");
    }
}

float takeTempMeasurement() {
    uint8_t i;
    float average;
    float steinhart;

    // take N samples in a row, with a slight delay
    for (i = 0; i < NUMSAMPLES; i++) {
        samples[i] = analogRead(THERMISTORPIN);
        delay(10);
    }

    // average all the samples out
    average = 0;
    for (i = 0; i < NUMSAMPLES; i++) {
        average += samples[i];
    }
    average /= NUMSAMPLES;

    if (DEBUG) {
        Serial.print("Average analog reading ");
        Serial.print(average);
    }

    // convert the value to resistance
    average = 1023 / average - 1;
    average = SERIESRESISTOR / average;

    if (DEBUG) {
        Serial.print(" Thermistor resistance ");
        Serial.print(average);
    }

    steinhart = log(average / THERMISTORNOMINAL);      // log(R/Ro)
    steinhart = (1.0 / (TEMPERATURENOMINAL + 273.15)) +
                ((1.0 / BCOEFFICIENT) * steinhart);    // (1/TO) + 1/B * ln(R/Ro)
    steinhart = 1.0 / steinhart;                       // Invert
    steinhart = steinhart - 273.15;

    if (DEBUG) {
        Serial.print(" Temperature ");
        Serial.print(steinhart);
        Serial.println(" *C");
    }
    return steinhart;
}

float readBatteryVoltage() {
    // Read 1.1V reference against AVcc
    // set the reference to Vcc and the measurement to the internal 1.1V reference
    #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ADMUX = _BV(MUX5) | _BV(MUX0);
    #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = _BV(MUX3) | _BV(MUX2);
    #else
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #endif

    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Start conversion
    while (bit_is_set(ADCSRA,ADSC)); // measuring

    uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
    uint8_t high = ADCH; // unlocks both

    float result = (high<<8) | low;

    result = 0.97 * 1023 / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000

    int average = 0;
    for (int i=0; i < 20; i++) {
        average += + analogRead(ADCVOLT);
    }
    int sensorValue = average/20;
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    float voltage = sensorValue * (result / 1023.0) * 2.2; // 2.2 for taking into account the resistor divider
    return voltage;
}

void setup() {
    pinMode(MOSFETPIN, OUTPUT);
    digitalWrite(MOSFETPIN, HIGH);  // keep mosfet ON
    pinMode(BUZZERPIN, OUTPUT);

    Serial.begin(9600);
    for (int i = 0; i < 10; i++) {  // throw away first 10 readings
        analogRead(ADCVOLT);
        delay(1);
    }

    // button.attachLongPressStart(changeMenu_longPressStart);
    button.attachClick(toggleBuzzer_singlePress);

    // take initial reading
    takeTempMeasurement();
}

void loop() {
    button.tick();
    unsigned long currentMillis = millis();
    float steinhart;

    if (currentMillis - previousMillis >= INTERVAL) {
        previousMillis = currentMillis;

        steinhart = takeTempMeasurement();

        // Milk should be brought up to 90C or 194F and then back down to 49C or 120F
        // If you introduce yogurt to higher than 49C or 120F it will kill all its cultures
        // Bringing the milk to almost a boil is not entirely necessary because every Milk
        // sold in stores will already be pasteurized. I do this because I have had better Yogurt done this way.
        if (steinhart > 90.0f && buzzer == ON) {
            // ring alarm, milk is hot enough for pasteurization
            digitalWrite(BUZZERPIN, HIGH);
            hasMilkWarmedUp = true;
            if (DEBUG) Serial.println("ALARM:pasteurized");
        } else if (hasMilkWarmedUp && steinhart < 51.0f && buzzer == ON) {
            // ring alarm, milk has cooled down enough
            digitalWrite(BUZZERPIN, HIGH);
            if (DEBUG) Serial.println("ALARM:ready for yogurt culture");
        }
        if (DEBUG) {
            Serial.print("Battery voltage: ");
            Serial.println(readBatteryVoltage(), 2);
        }

        if (currentMillis - idleMillis >= IDLEINTERVAL) {
            idleMillis = currentMillis;
            if (!firstRun) {
                float diff = fabs(steinhart - oldTemperatureValue);
                if (diff <= 2.0f) {
                    // if temp has not changed 2 degrees in 5 min then we are not measuring anything
                    digitalWrite(MOSFETPIN, LOW);
                }
            }
            if (firstRun) {
                firstRun = false;
            }
            oldTemperatureValue = steinhart;
        }
    }

    if (buzzer == OFF) digitalWrite(BUZZERPIN, LOW);

    if (currentMillis - buzzerMillis >= BUZZERINTERVAL) {
        buzzerMillis = currentMillis;
        // Always reset the buzzer to ON just in case we turn off the buzzer but we never
        // turn off the stove. In that case the buzzer will start again.
        buzzer = ON;
        if (DEBUG) Serial.println("Buzzer: RESET TO ON");
    }
}
