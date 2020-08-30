//Bluetooth Globe Display
//Zia Formuly
/*Info
 * Wiring
 * HALL:
 * Vcc - 220Ohm Resistor - Arduino 5V
 * GND - Arduino Ground
 * Data - 220Ohm Resistor - Arduino D2
 * 
 * LED STRIP:
 * Vcc - Battery 5V
 * GND - Battery Ground
 * Data - 220Ohm Resistor - Arduino D5
 * 
 * HM-10:
 * BT VCC - Battery 5V 
 * BT GND - GND
 * BT TX - Arduino D8 (SS RX)
 * BT RX (through voltage divider 5V to 3.3V) - Arduino D9 (SS TX)
 * 
 * Other:
 * Battery 5V - Diode - Arduino Vin
 * 1000uF Cap across battery 5V and ground
 *
 * Bluetooth Codes:
 * Reset - Re
 * Off - Of
 * Custom Color - C(0x000000)
 * Image - I(0-6)
 * Animation - A(0-4)
 * Brightness - B(0-255)
 * Phone Orientation  - P(000-360) (00-90) (00-90)
 * 
 * Bug:
 * Must send code twice when switching between images/animations or out from images/animations
 *    -Occurs because FastLED corrupts the serial code which stops image/animation
 */
#include <AltSoftSerial.h>
#include <FastLED.h>

void(* resetFunc) (void) = 0;   //Reset function

#define LED_PIN 5
#define NUM_LEDS 46
#define BRIGHTNESS 25
CRGB leds[NUM_LEDS];

AltSoftSerial BTserial; 

/*Images and color pallet arrays
 * Image 0: Grid (red)
 * Image 1: Map Black and Green
 * Image 2: Earth Color
 * Image 3: Moon
 * Image 4: Globe (green)
 * Image 5: Zia Globe (green)
 * Image 6: Globe Display (green)
 */
const long int clr[][14] = {{0x320000, 0x000000},
                            {0x000000, 0x003300, 0x336633, 0x333333, 0x666666},
                            {0x000033, 0x330033, 0x999999, 0x000033, 0x808080, 0x333300, 0x003300, 0x666666, 0x333333, 0x666633, 0xFFFFFF, 0xCCCC99, 0x999966, 0x996666},
                            {0x686868, 0x4E4E4E, 0x353535, 0x1A1A1A, 0x868686},
                            {0x000000, 0x003200},
                            {0x000000, 0x003200, 0x003200},
                            {0x000000, 0x003200}
                            };
const byte pixel[][41][23] PROGMEM = {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1}},
                                      {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 3, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 4, 3, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 3, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1, 3, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0}, {3, 1, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 3, 0, 0}, {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 3, 0, 1, 3, 1, 3, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 3, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0}, {3, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {1, 3, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0}, {1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0}},
                                      {{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, {0, 0, 0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4}, {0, 0, 0, 5, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 2}, {0, 0, 0, 1, 5, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 7, 2}, {0, 0, 3, 6, 5, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 2, 4}, {0, 3, 3, 3, 5, 5, 3, 3, 3, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 1, 2, 2}, {3, 3, 8, 1, 5, 6, 6, 1, 6, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 8, 2, 2}, {1, 8, 8, 8, 5, 6, 5, 9, 7, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 8, 2, 2}, {8, 8, 8, 8, 5, 5, 5, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 2, 4}, {8, 7, 8, 8, 5, 6, 6, 5, 9, 5, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 2}, {7, 4, 7, 1, 8, 3, 6, 5, 6, 0, 6, 3, 0, 0, 3, 3, 3, 0, 3, 3, 7, 2, 2}, {2, 7, 7, 8, 3, 1, 6, 6, 6, 3, 3, 1, 1, 0, 3, 0, 3, 3, 3, 3, 7, 2, 7}, {7, 8, 3, 7, 8, 1, 6, 6, 0, 3, 6, 6, 5, 8, 9, 8, 8, 3, 3, 7, 2, 4, 7}, {10, 2, 3, 3, 3, 1, 0, 0, 0, 0, 3, 6, 6, 5, 5, 1, 0, 3, 8, 7, 7, 2, 8}, {10, 10, 2, 7, 8, 3, 3, 0, 0, 0, 0, 6, 5, 5, 1, 0, 3, 0, 3, 3, 3, 8, 7}, {10, 10, 10, 10, 10, 3, 3, 3, 0, 3, 3, 0, 5, 5, 3, 0, 0, 3, 0, 3, 3, 3, 7}, {10, 10, 10, 10, 1, 3, 3, 3, 0, 3, 3, 3, 3, 3, 0, 0, 3, 0, 0, 3, 3, 3, 7}, {10, 10, 2, 7, 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 0, 0, 0, 0, 0, 3, 3, 7, 7}, {8, 8, 3, 3, 1, 3, 3, 3, 3, 8, 5, 3, 3, 0, 0, 0, 0, 0, 3, 3, 7, 2, 7}, {3, 3, 3, 3, 3, 3, 3, 3, 8, 11, 9, 6, 3, 0, 0, 0, 0, 0, 0, 3, 2, 2, 2}, {3, 3, 3, 3, 3, 0, 6, 5, 12, 11, 13, 6, 0, 3, 0, 3, 0, 0, 3, 8, 2, 2, 2}, {1, 3, 3, 3, 0, 0, 5, 1, 12, 11, 13, 6, 6, 3, 3, 0, 0, 0, 3, 8, 2, 2, 2}, {7, 1, 3, 3, 6, 6, 6, 5, 8, 11, 13, 5, 5, 9, 1, 3, 0, 0, 0, 8, 2, 2, 2}, {8, 3, 3, 1, 6, 6, 6, 5, 9, 11, 13, 5, 5, 9, 1, 0, 0, 0, 3, 8, 2, 2, 2}, {0, 3, 3, 0, 6, 6, 5, 5, 7, 12, 9, 9, 5, 5, 0, 0, 0, 0, 3, 7, 2, 2, 2}, {0, 3, 3, 0, 6, 6, 5, 9, 12, 11, 13, 5, 0, 1, 0, 0, 0, 0, 3, 2, 2, 2, 4}, {0, 3, 8, 3, 6, 6, 5, 13, 12, 12, 9, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2}, {0, 8, 3, 1, 6, 6, 9, 12, 12, 9, 0, 0, 0, 0, 0, 0, 0, 3, 3, 2, 7, 2, 2}, {0, 1, 3, 5, 6, 5, 9, 9, 13, 13, 6, 0, 0, 0, 0, 0, 0, 0, 3, 7, 7, 4, 4}, {0, 3, 1, 5, 6, 6, 9, 7, 13, 9, 5, 0, 0, 0, 0, 0, 0, 3, 3, 4, 2, 2, 4}, {3, 3, 8, 5, 5, 6, 5, 12, 13, 9, 6, 0, 0, 0, 0, 0, 0, 3, 3, 2, 2, 2, 7}, {7, 8, 9, 5, 5, 6, 5, 12, 9, 9, 6, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 7, 2}, {8, 8, 8, 5, 5, 6, 5, 12, 9, 5, 6, 6, 3, 0, 0, 0, 0, 3, 1, 2, 2, 2, 2}, {0, 1, 5, 5, 5, 5, 5, 13, 5, 5, 0, 0, 0, 1, 1, 0, 0, 0, 3, 2, 10, 2, 2}, {0, 3, 1, 5, 5, 5, 5, 5, 6, 6, 0, 0, 1, 9, 1, 0, 0, 0, 1, 2, 11, 2, 2}, {0, 3, 3, 5, 5, 5, 6, 6, 0, 0, 3, 0, 5, 9, 1, 0, 0, 0, 8, 2, 2, 2, 4}, {0, 1, 3, 5, 5, 6, 6, 0, 0, 0, 0, 0, 1, 13, 8, 0, 0, 0, 3, 2, 2, 2, 4}, {0, 3, 0, 5, 5, 1, 0, 0, 0, 0, 0, 0, 0, 5, 1, 0, 0, 0, 3, 2, 2, 2, 2}, {0, 0, 3, 5, 5, 6, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 7, 2, 2, 4}, {0, 0, 0, 1, 5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 7, 3, 2}, {0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 2}},
                                      {{0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 0}, {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 2, 2, 2, 2, 0, 1}, {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 2, 2, 1, 1, 0}, {0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 2, 1, 1, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 1, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1}, {1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 1, 2, 1, 1, 1}, {0, 0, 1, 1, 2, 1, 2, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0, 1}, {0, 1, 0, 1, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 1, 2, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 1}, {0, 0, 0, 1, 1, 2, 2, 3, 2, 2, 2, 1, 2, 2, 1, 2, 1, 1, 1, 1, 1, 0, 2}, {0, 0, 1, 1, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 2, 1, 1, 1, 0, 1, 1}, {0, 0, 0, 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 2, 1, 1, 2, 1, 0, 1, 0}, {1, 0, 0, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 2, 2, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 0, 0, 1}, {0, 0, 0, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 2, 1, 0, 1, 1}, {0, 0, 1, 2, 2, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 2, 1, 1, 1, 1, 1, 0, 1}, {0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 2, 1, 1, 0, 0, 0, 0, 1}, {0, 0, 0, 2, 2, 2, 3, 3, 3, 3, 3, 3, 2, 3, 2, 1, 0, 0, 1, 0, 0, 0, 1}, {0, 0, 0, 1, 2, 2, 2, 3, 2, 3, 3, 2, 2, 2, 2, 1, 0, 1, 0, 1, 0, 0, 2}, {1, 0, 0, 1, 2, 2, 2, 3, 3, 3, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 0, 0, 1}, {1, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 2, 2, 1, 2, 1, 1, 1, 1, 1, 0, 4, 1}, {1, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 2, 1, 1, 1, 1, 2, 1, 1, 0, 0, 2}, {1, 0, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 2, 3, 2, 2, 1, 1, 1, 1, 0, 0, 1}, {0, 1, 1, 1, 1, 1, 2, 1, 2, 2, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0}, {1, 0, 1, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 2, 0, 1, 1, 2, 0, 0, 1, 1}, {1, 0, 1, 0, 1, 2, 2, 2, 2, 3, 2, 3, 2, 2, 2, 1, 2, 1, 1, 1, 1, 0, 2}, {0, 1, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 1, 1, 1, 2}, {1, 1, 0, 0, 1, 2, 1, 1, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 0, 2}, {0, 1, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 2, 1, 2, 2, 2, 2, 2, 2, 1, 0, 1}, {0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 1, 2, 1, 2, 2, 2, 2, 1, 2, 1, 0}, {1, 1, 0, 0, 0, 1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2, 2, 1, 1, 1, 0}, {1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1}, {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 2, 1, 1, 1, 1, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2, 1, 2, 1, 1, 0, 0}, {4, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1}, {0, 0, 1, 1, 2, 1, 1, 2, 2, 2, 1, 1, 2, 2, 1, 2, 2, 1, 2, 1, 1, 0, 1}, {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 0, 1, 0}, {0, 0, 1, 1, 1, 2, 1, 1, 1, 1, 0, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1, 0, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1}},
                                      {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                      {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                      {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
                                      };

//To track the column order to make image rotate
byte columnOrder[41] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 ,26, 27 ,28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
byte tempColumn = 0;

long int tempClr = 0;     //Variable to hold current color to show in 2 leds
bool run = false;         //Hall sensor variable

//Bluetooth Variables
String BTString = " ";
char c = ' ';
byte length = 30;
char buffer [31];
char termChar = '\n';
byte index = 0;
boolean haveNewData = false;

byte r, g, b;

//BT Serial related variables
char char1 = ' '; 
int num = 0;
bool imageCom = false;
bool animationCom = false;

//Used for animation
int counter;

//Animations array
typedef void (*AnimationFunctionArray)();
AnimationFunctionArray animationFunction[5] = {&animation0, &animation1, &animation2, &animation3, &animation4};

bool rotate = true;

void setup() {
   BTserial.begin(9600);  
   
   //LED setup
   FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
   FastLED.setBrightness(BRIGHTNESS);
   
   //Hall sensor setup
   pinMode(2, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(2), runLEDS, FALLING);

   runStartUpAnimation();
}

void loop() {
  readSerial();         //Checks and reads the BT serial
  if ( haveNewData )    //If end of command is reached store the full command and calls switch case
       processNewData();

  //This checks if the command is for an image or animation which need to run repeatedly
  if(imageCom)
      displayImage(num);
  else if(animationCom)
    animationFunction[num]();
}

//Calls the appropriate functions based on BT data
void switchCase(char i)
{
  switch(i)
  {
      case 'I':                         //Image - I(0-6)
      imageCom = true;
      animationCom = false;
      break;
            
      case 'A':                         //Animation - A(0-4)
      imageCom = false;
      animationCom = true;
      break;
      
      case 'C':                         //Custom Color - C(0x000000)
      imageCom = false;
      animationCom = false;
      fill_solid(leds, NUM_LEDS, (strtol((BTString.substring(1)).c_str(), NULL, 16))); FastLED.show();    //Fills leds with the sent color from phone bluetooth
      break;
      
      case 'B':                         //Brightness - B(0-255)
      imageCom = false;
      animationCom = false;
      FastLED.setBrightness(BTString.substring(1).toInt()); FastLED.show();
      break;
      
      case 'P':                       //Phone Orientation - P(0-360) (0-90) (0-90)
      imageCom = false;
      animationCom = false;
      r = map(BTString.substring(1,4).toInt(), 0, 360, 0, 128);     //Maps each phone orientation axis to a rgb value
      g = map(BTString.substring(5,7).toInt(), 0, 90, 0, 128);
      b = map(BTString.substring(8,10).toInt(), 0, 90, 0, 128);
      fill_solid(leds, NUM_LEDS, CRGB(r, g, b)); FastLED.show();    //Fills leds to that color
      break;
      
      case 'O':                      //Off - O
      imageCom = false;
      animationCom = false;
      fill_solid(leds, NUM_LEDS, CRGB::Black); FastLED.show();                  //Turns off leds
      break;
      
      case 'R':                     //Reset - R
      imageCom = false;
      animationCom = false;
      resetFunc();
      break;
      
      default:
      break;
  }
}

//Method which displays an image using i as an indicator to which image
void displayImage(int i)
{
  if (run)
  {
    for (int x = 0; x < (sizeof(pixel[0])/sizeof(pixel[0][0])); x++)
    {
      for (int y = 0; y < (sizeof(pixel[0][0]) * 2); y += 2)
      {
        tempClr = clr[i][pgm_read_byte_near(&pixel[i][columnOrder[x]][(y/2)])];    //Stores the color into tempClr variable
        leds[y] = tempClr;                                                         //Writes the tempclr to two leds
        leds[y+1] = tempClr;
      }
      FastLED.show();
    }
    
    fill_solid(leds, NUM_LEDS, CRGB::Black);    //This shows black when the array of pixels runs out
    FastLED.show();
    if(rotate)                                  //This bool slows down rotation by 1/2
        shiftColumns();                         //Shifts column order to rotate image
    rotate = !rotate;
    run = false;                                //Resets bool
  }
}

//This reads the bluetooth serial
void readSerial()
{
    if (BTserial.available()) 
    {         
       c = BTserial.read();
       
       if (c != NULL)           //This resets the bools to stop them (need the if statement because this event is called once after command)    
       {
          imageCom = false;
          animationCom = false;
       }
       
       if (c != termChar)
       {
         buffer[index] = c;
         index = index + 1;
       }
       else
       {
         buffer[index] = '\0';
         if(index > 1)
            haveNewData = true;
         index = 0;
       }
    }
}

//This runs once a complete string is read from bluetooth serial
void processNewData()
{
  BTString = buffer;                    //Store full recieved string
  char1 = BTString.charAt(0);           //Stores the first char of string
  num = BTString.charAt(1) - '0';       //Store the second char of string (number)
  switchCase(char1);                    //Calls switch case
  haveNewData = false;                  //Resets bool
}


//Functions which store animations
//RGB Fade in/out Animation
void animation0()
{
  RGBLoop();
}
//Moving Lights Animation
void animation1()
{
  chase(5);
}
//Fire Animation *not very good
void animation2()
{
  Fire(90,120,1);
}
//Rainbow Animation
void animation3()
{
  rainbowCycle(1);
}
//Chasing Lights Animation
void animation4()
{
  theaterChaseRainbow(1);
}

//Hall Interrupt ISR
void runLEDS()
{
  run = true;
}

//Start-up animation
void runStartUpAnimation()
{
    for (int i = 46; i > 0; i--)
  {
    leds[i] = CRGB::White;
    FastLED.delay(25);
  }
  delay(200);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.delay(100);
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.delay(100);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.delay(100);
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.delay(750);
}

//Shifts the columns in columnOrder array over by one
void shiftColumns()
{
  tempColumn = columnOrder[0];                        //remember first element
  for(int i=0; i < (sizeof(columnOrder) - 1); i++)
  {
    columnOrder[i] = columnOrder[i+1];                //move all element to the left except first one
  }
  columnOrder[sizeof(columnOrder)-1] = tempColumn;    //assign remembered value to last element
}

/*********Animations - Source: https://www.tweaking4all.com/hardware/arduino/arduino-all-ledstrip-effects-in-one/ *******************************************************/
//Chase
void chase(int SpeedDelay){
  static uint8_t hue = 0;

    for (int i = 0; i < NUM_LEDS; i++) {
    if (counter % 2 == 0 && counter != 0) {
      hue += 10; //change color each step
    } else {
      hue == hue;
    }
    leds[i] = CHSV(hue, 255, 255);
    FastLED.show();
    for (int j = 0; j < NUM_LEDS; j++) {
      leds[j].nscale8(90); //fading tail 4 leds
    }
    delay(SpeedDelay);
    }
    for (int i = NUM_LEDS; i > 0; i--) {
    if (counter % 2 == 0 && counter != 0) {
      hue += 10; //change color each step
    } else {
      hue == hue;
    }
    leds[i] = CHSV(hue, 255, 255);
    FastLED.show();
    for (int j = NUM_LEDS; j > 0; j--) {
      leds[j].nscale8(90); //fading tail 4 leds
    }
    delay(SpeedDelay);
    }
    counter++;
}

//Fire
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(5);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  for (int i = 0, j = NUM_LEDS - 1; i < NUM_LEDS/2; i++, j--)
  {
    int temp = heat[i];
    heat[i] = heat[j];
    heat[j] = temp;
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

    for (int i = 0, j = NUM_LEDS - 1; i < NUM_LEDS/2; i++, j--)
  {
    int temp = heat[i];
    heat[i] = heat[j];
    heat[j] = temp;
  }

  FastLED.delay(SpeedDelay);
}
void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    leds[Pixel].setRGB(255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    leds[Pixel].setRGB(255, heatramp, 0);
  } else {                               // coolest
    leds[Pixel].setRGB(heatramp, 0, 0);
  }
}

//Chasing Lights
void theaterChaseRainbow(int SpeedDelay) {
  byte *c;
 
  for (int j=0; j < 256; j++) {                     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < NUM_LEDS; i=i+3) {
          c = Wheel( (i+j) % 255);
          leds[i+q].setRGB(*c, *(c+1), *(c+2));     //turn every third pixel on
        }
        FastLED.delay(SpeedDelay);
       
        for (int i=0; i < NUM_LEDS; i=i+3) {
          leds[i+q].setRGB(0, 0, 0);                //turn every third pixel off       
        }
    }
  }
}
byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

//Rainbow
void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel1(((i * 256 / NUM_LEDS) + j) & 255);
      leds[i].setRGB(*c, *(c+1), *(c+2));
    }
    FastLED.delay(SpeedDelay);
  }
}
byte * Wheel1(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

//RGB Fade in/out
void RGBLoop(){
  for(int j = 0; j < 3; j++ ) {
    // Fade IN
    for(int k = 0; k < 256; k++) {
      switch(j) {
        case 0: fill_solid(leds, NUM_LEDS, CRGB(k,0,0)); break;
        case 1: fill_solid(leds, NUM_LEDS, CRGB(0,k,0)); break;
        case 2: fill_solid(leds, NUM_LEDS, CRGB(0,0,k)); break;
      }
      FastLED.delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) {
      switch(j) {
        case 0: fill_solid(leds, NUM_LEDS, CRGB(k,0,0)); break;
        case 1: fill_solid(leds, NUM_LEDS, CRGB(0,k,0)); break;
        case 2: fill_solid(leds, NUM_LEDS, CRGB(0,0,k)); break;
      }
      FastLED.delay(3);
    }
  }
}
