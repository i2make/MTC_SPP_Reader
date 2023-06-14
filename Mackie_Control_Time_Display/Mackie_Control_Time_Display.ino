#include <MIDIUSB.h>      // https://github.com/arduino-libraries/MIDIUSB @ ^1.0.5
#include "LedControl.h"   // https://github.com/wayoda/LedControl @ ^1.0.6

#define MAX7219_CS                  10
#define MAX7219_DATA                16
#define MAX7219_CLOCK               15
#define BUTTON                      14      // SMPTE/BEATS change button
#define MAX7219_DEVICE_NUMBER        2

#define MTC_LED_NUMBER               0      // _000__00
#define SPP_LED_NUMBER               1      //         __00_000

String displayString; // display string
midiEventPacket_t rx; // Midi Read

// Hardware SPI setting
LedControl lc(MAX7219_DATA, MAX7219_CLOCK, MAX7219_CS, MAX7219_DEVICE_NUMBER);

// functions
void displayPosition();
void buttonPressed();
void noteOn(byte channel, byte pitch, byte velocity);


void setup() {
    pinMode(MAX7219_DATA, OUTPUT);
    pinMode(MAX7219_CLOCK, OUTPUT);
    pinMode(MAX7219_CS, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    // The MAX72XX is in power-saving mode on startup,
    for (int i = 0; i < MAX7219_DEVICE_NUMBER; i++) {
        lc.shutdown(i, false);      // wakeup call
        lc.setIntensity(i, 8);  // Set the brightness to a medium values
        lc.clearDisplay(i);             // clear the display
    }
}

void loop() {
    buttonPressed();
    rx = MidiUSB.read();
    displayPosition();
} // loop()

////////////////////////////////////////////////////
/// functions
////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// rx.byte2 : display location
///    0  0  0  0  0  0  0  0  0  0 : 10-digit 7-segments display
/// 0x49 48 47 46 45 44 43 42 41 40
///
/// rx.byte3 : data
/// 0000 0000
///  ^ ^ ^^^^
///  | | number
///  | (_ or 0) display (cubase only)
///  point
//////////////////////////////////////////////////////////////
void displayPosition() {
    if (rx.byte1 >> 4 == 0x0B) {                        // 0xB_ : control change
        if (rx.byte2 >= 0x40 && rx.byte2 <= 0x42) {     // position display message
            if ((rx.byte3 & 0x10) == 0) {
                displayString = " ";
            } else {
                displayString = rx.byte3 & 0x0F;
            }
            lc.setChar(SPP_LED_NUMBER, rx.byte2 & 0x0F, displayString.charAt(0), rx.byte3 & 0x40);
        }
        if (rx.byte2 >= 0x43 && rx.byte2 <= 0x44) {     // position display message
            if ((rx.byte3 & 0x10) == 0) {
                displayString = " ";
            } else {
                displayString = rx.byte3 & 0x0F;
            }
            lc.setChar(SPP_LED_NUMBER, (rx.byte2 & 0x0F) + 1, displayString.charAt(0), rx.byte3 & 0x40);
        }
        if (rx.byte2 >= 0x45 && rx.byte2 <= 0x46) {     // position display message
            if ((rx.byte3 & 0x10) == 0) {
                displayString = " ";
            } else {
                displayString = rx.byte3 & 0x0F;
            }
            lc.setChar(MTC_LED_NUMBER, (rx.byte2 & 0x0F) - 5, displayString.charAt(0), rx.byte3 & 0x40);
        }
        if (rx.byte2 >= 0x47 && rx.byte2 <= 0x49) {     // position display message
            if ((rx.byte3 & 0x10) == 0) {
                displayString = " ";
            } else {
                displayString = rx.byte3 & 0x0F;
            }
            lc.setChar(MTC_LED_NUMBER, (rx.byte2 & 0x0F) - 3, displayString.charAt(0), rx.byte3 & 0x40);
        }
    }
}

void buttonPressed() {
    static byte previousButton = 0;
    static uint16_t debounce = 0;
    byte inputButton = digitalRead(BUTTON);
    debounce = (debounce << 1) | inputButton;
    delay(1);

    if (debounce == 0 && previousButton == 0) {
        noteOn(0, 0x35, 0x7F);
        MidiUSB.flush();
        previousButton = 1;
    }

    if (debounce == 0xFFFF && previousButton == 1) {
        noteOn(0, 0x35, 0);
        MidiUSB.flush();
        previousButton = 0;
    }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}