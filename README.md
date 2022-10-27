# Midi Time Code(MTC) & Song Position Pointer(SPP) Reader  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/pic2.png)
## Using boards

- Arduino Pro Micro x 1  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/arduino_pro_micro.png)
- MAX7219 8-digit 7-segment led x 2  
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/max7219_7-segment.png)

## Dependencies

- MIDIUSB.h
- LedControl.h

## Install Dependencies


## Compile

- Define Pins

```c++
#define MAX7219_CS                  10
#define MAX7219_DATA                16
#define MAX7219_CLOCK               15
#define MAX7219_DEVICE_NUMBER        2
```

## Cubase Configuration
![picture](https://github.com/i2make/MTC_SPP_Reader/blob/main/cubass.png)
