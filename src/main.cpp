
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

#include <Arduino.h>
#include <SevSeg.h>
#include <OneButton.h>

#define DEBUG 1
#define THERMISTORPIN A0
#define BUTTONPIN A1
#define BUZZERPIN A2
#define THERMISTORNOMINAL 9300  // resistance at 25 degrees C
#define TEMPERATURENOMINAL 25   // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 5            // how many samples to take and average, more takes longer
#define BCOEFFICIENT 3380       // The beta coefficient of the thermistor (usually 3000-4000)
#define SERIESRESISTOR 9000              // resistor value of voltage divider
#define INTERVAL 1000UL * 10             // interval at which to take a temp measurement
#define BUZZERINTERVAL 1000UL * 60 * 2   // interval at which a buzzer will be reset back to ON - 2 min

typedef enum { KOS, PUL, VIC, DER } Menu;                   // menu options
static char* MenuText[] = { "KOS", "PUL", "VIC", "DER", };  // printable string representation of menu
typedef enum { ON, OFF } Buzzer;                            // buzzer options

int samples[NUMSAMPLES];
float steinhart;
unsigned long previousMillis = 0;     // will store last time a temp measurement was updated
unsigned long buzzerMillis = 0;       // will store last time a buzzer was reset
bool hasMilkWarmedUp = false;

Menu menu = KOS;                // Default menu to KOS when we start
OneButton button(BUTTONPIN, 1);        // button when pressed will take A1 to ground, active low
Buzzer buzzer = ON;            // Initial buzzer status; allow the buzzer to sound by default
SevSeg sevseg;


/*
 * Callback that changes the menu state. Default will start at KOS and with each
 * new long press it will cycle through the menu options.
 */
void changeMenu_longPressStart() {
    if (menu == KOS) menu = PUL;
    else if (menu == PUL) menu = VIC;
    else if (menu == VIC) menu = DER;
    else if (menu == DER) menu = KOS;

    for (int i = 0; i < 1000; ++i) {
        sevseg.setChars(MenuText[menu]);
        sevseg.refreshDisplay();
        delay(1);
    }
    for (int j = 0; j < 500; ++j) {
        sevseg.setChars("   -");
        sevseg.refreshDisplay();
        delay(1);
    }
}

void toggleBuzzer_singlePress() {
    if (buzzer == ON) {
        buzzer = OFF;
        hasMilkWarmedUp = false;
        Serial.println("Buzzer: OFF");
    }
    else {
        buzzer = ON;
        Serial.println("Buzzer: ON");
    }
}

void takeTempMeasurement() {
    uint8_t i;
    float average;

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

    steinhart = log(average / THERMISTORNOMINAL);       // log(R/Ro)
    steinhart = (1.0 / (TEMPERATURENOMINAL + 273.15)) +
                ((1.0 / BCOEFFICIENT) * steinhart);    // (1/TO) + 1/B * ln(R/Ro)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart = steinhart - 273.15;                         // convert to C

    if (DEBUG) {
        Serial.print(" Temperature ");
        Serial.print(steinhart);
        Serial.println(" *C");
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(BUZZERPIN, OUTPUT);

    byte numberOfDigits = 4;
    byte digitPins[] = {10, 11, 12, 13};
    byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9};

    sevseg.begin(COMMON_ANODE, numberOfDigits, digitPins, segmentPins);
    sevseg.setBrightness(50);

    button.attachLongPressStart(changeMenu_longPressStart);
    button.attachClick(toggleBuzzer_singlePress);

    // take initial reading
    takeTempMeasurement();
}

void loop() {
    button.tick();
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= INTERVAL) {
        previousMillis = currentMillis;

        takeTempMeasurement();

        if (menu == KOS) {
            // Milk should be brought up to 90C or 194F and then back down to 49C or 120F
            // If you introduce yogurt to higher than 49C or 120F it will kill all its cultures
            // Bringing the milk to almost a boil is not entirely necessary because every Milk
            // sold in stores will already be pasteurized. I do this because I have had better Yogurt done this way.
            if (steinhart > 49.0f) {
                hasMilkWarmedUp = true;
                if (DEBUG) Serial.println("milk has warmed up");
            }
            if (steinhart > 90.0f) {
                // ring alarm, milk is hot enough for pasteurization
                if (buzzer == ON) {
                    digitalWrite(BUZZERPIN, HIGH);
                    if (DEBUG) Serial.println("ALARM:pasteurized");
                }
            } else if (hasMilkWarmedUp && steinhart < 49.0f) {
                // ring alarm, milk has cooled down enough
                if (buzzer == ON) {
                    digitalWrite(BUZZERPIN, HIGH);
                    if (DEBUG) Serial.println("ALARM:ready for yogurt culture");
                }
            }

        } else if (menu == PUL) {
            // Chicken 74C 165F
        } else if (menu == VIC) {
            // Beef Medium 58C 135F
        } else if (menu == DER) {
            // Pork 65C 150F
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

    sevseg.setNumber(steinhart, 1);
    sevseg.refreshDisplay();
}