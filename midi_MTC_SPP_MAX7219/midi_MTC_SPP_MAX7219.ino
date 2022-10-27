#include <MIDIUSB.h>
#include "LedControl.h" // https://github.com/wayoda/LedControl

#define MAX7219_CS                  10
#define MAX7219_DATA                16
#define MAX7219_CLOCK               15
#define MAX7219_DEVICE_NUMBER        2

#define MTC_LED_NUMBER               0
#define SPP_LED_NUMBER               1

// starting message font
char mtcFont[] = {0b00001000, 0b00001000, 0b00001000, 0b01010100,
                  0b00001111, 0b00001101, 0b00001000, 0b00001000}; //"___mtc__"
char sppFont[] = {0b00001000, 0b00001000, 0b00001000, 0b01011011,
                  0b01100111, 0b01100111, 0b00001000, 0b00001000}; //"___spp__";

String displayString; // display string
midiEventPacket_t rx; // Midi Read

// Song Position Pointer(SPP)
unsigned int sixteens;
unsigned int bars;
unsigned char quarters;
unsigned int sixteen;
int ppqn;

// Midi Time Code(MTC)
enum {
    F24 = 0, F25 = 2, F30DF = 4, F30 = 6
}; // Frames type
unsigned char h, m, s, f;      // hour, minutes, seconds, frames time code
unsigned char frameType;       // speed of MTC : 24fps / 25fps / 30 drop frame / 30 fps
unsigned char tc[7];          // array to old timecode

// Hardware SPI setting
LedControl lc(MAX7219_DATA, MAX7219_CLOCK, MAX7219_CS, MAX7219_DEVICE_NUMBER);

// functions
void displayStartingMessage();

void midiReadSPP();

void midiReadMTC();


void setup() {
    pinMode(MAX7219_DATA, OUTPUT);
    pinMode(MAX7219_CLOCK, OUTPUT);
    pinMode(MAX7219_CS, OUTPUT);

    // The MAX72XX is in power-saving mode on startup,
    for (int i = 0; i < MAX7219_DEVICE_NUMBER; i++) {
        lc.shutdown(i, false);      // wakeup call
        lc.setIntensity(i, 8);  // Set the brightness to a medium values
        lc.clearDisplay(i);             // clear the display
    }

    // display starting message
    displayStartingMessage();
}

void loop() {
    rx = MidiUSB.read();
    midiReadSPP();
    midiReadMTC();
} // loop()

////////////////////////////////////////////////////
/// functions
////////////////////////////////////////////////////

void displayStartingMessage() {
    for (int i = 0; i < 8; i++) {
        lc.setRow(0, 7 - i, mtcFont[i]);
        lc.setRow(1, 7 - i, sppFont[i]);
    }
    delay(2000);
}

void displaySPP() {
    bars = int(sixteens / 16) + 1;
    quarters = (sixteens % 16) / 4 + 1;
    sixteen = (sixteens % 16) % 4 + 1;
    switch (bars) {
        case 0 ... 9:
            displayString = "   " + String(bars);
            break;
        case 10 ... 99:
            displayString = "  " + String(bars);
            break;
        case 100 ... 999:
            displayString = " " + String(bars);
            break;
        default:
            displayString = String(bars);
            break;
    }
    displayString += " " + String(quarters) + " " + String(sixteen);

    for (int i = 0; i < 8; i++) {
        char temp = displayString.charAt(i);
        lc.setChar(SPP_LED_NUMBER, 7 - i, temp, false);
    }
}

void midiReadSPP() {
    if (rx.byte1 == 0xF2) {
        sixteens = rx.byte3 * 128 + rx.byte2; // total sixteens
        displaySPP();
    }
    if (rx.byte1 == 0xF8) {
        ppqn++;
        if (ppqn > 5) {
            sixteens++; // add total sixteens
            displaySPP();
            ppqn = 0;
        }
    }
}

void midiReadMTC() {
    ///////////////////////////////////////
    /// MTC data
    /// rx.byte1   rx.byte2
    /// 0xF1       data (x000 0000)
    ///                    ^    ^
    ///               indices  data
    ///                0 ~ 7
    ///////////////////////////////////////
    if (rx.byte1 == 0xF1) {
        int indices = (rx.byte2 & 0xF0) >> 4;   // indices, storing high 4-digit
        tc[indices] = rx.byte2 & 0x0F;          //    data, storing low 4-digit

        if (indices == 7) {
            //h = (tc[7] & 0x01) * 16 + tc[6];
            m = tc[5] * 16 + tc[4];
            s = tc[3] * 16 + tc[2];
            f = tc[1] * 16 + tc[0];

            switch (m) { /// display minutes
                case 0 ... 9:
                    displayString = String(" ") + m;
                    break;
                case 10 ... 99:
                    displayString = m;
                    break;
                default:
                    break;
            } //switch

            switch (s) { /// display seconds
                case 0 ... 9:
                    displayString += String("  ") + s;
                    break;
                case 10 ... 99:
                    displayString += String(" ") + s;
                    break;
                default:
                    break;
            } //switch

            switch (f) { /// display frames
                case 0 ... 9:
                    displayString += String("  ") + f;
                    break;
                case 10 ... 99:
                    displayString += String(" ") + f;
                    break;
                default:
                    break;
            } //switch

//                    frameType = tc[7] & 0x06;   // 0000 0110
//                    // 0=24fps, 2=25fps, 4=30(df)fps, 6=30fps
//                    switch (frameType) {
//                        case F24:
//                            displayString = "24 fps";
//                            break;
//                        case F25:
//                            displayString = "25 fps";
//                            break;
//                        case F30DF:
//                            displayString = "30df fps";
//                            break;
//                        case F30:
//                            displayString = "30 fps";
//                            break;
//                        default:
//                            displayString = "Error";
//                            break;
//                    }
            for (int i = 0; i < 8; i++) {
                char temp = displayString.charAt(i);
                lc.setChar(MTC_LED_NUMBER, 7 - i, temp, false);
            }
        } // if(indices == 7)
    } //if 0xF1
}
