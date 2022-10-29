# Midi Time Code(MTC) & Song Position Pointer(SPP) Reader  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/pic2.png)
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/schematics.png)
## Using boards

- Arduino Pro Micro x 1  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/arduino_pro_micro.png)
- MAX7219 8-digit 7-segment led x 2  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/max7219_7-segment.png)

## Dependencies

- MIDIUSB.h
- LedControl.h

## Install Dependencies  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/lib.png)  
- MIDIUSB.h  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/midiusb.png)  
- LedControl.h  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/ledcontrol.png)  

## Compile

- Define Pins

```c++
#define MAX7219_CS                  10
#define MAX7219_DATA                16
#define MAX7219_CLOCK               15
```

## Cubase Configuration  
menu Transport
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/cubase.png)
