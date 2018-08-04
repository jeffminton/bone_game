#include <Arduino.h>
#include <Keypad.h>
#include <LEDMatrixDriver.hpp>
#include <Keyboard.h>
// #include <Bounce2.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 4

#define ROW_1 7
#define ROW_2 8
#define ROW_3 9
#define ROW_4 10
#define ROW_5 11
#define ROW_6 12
#define COL_1 13
#define COL_2 14
#define COL_3 15
#define COL_4 16
#define COL_5 17
#define COL_6 18
#define COL_7 19
#define COL_8 20

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// This sketch will 'flood fill' your LED matrix using the hardware SPI driver Library by Bartosz Bielawski.
// Example written 16.06.2017 by Marko Oette, www.oette.info

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 0;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7
const int LEDMATRIX_WIDTH = 8;
const int LEDMATRIX_HEIGHT = 6;
const int LEDMATRIX_SEGMENTS = 1;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

const byte rows = 6; //four rows
const byte cols = 8; //three columns
char keys[rows][cols] = {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}, 
    {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'}, 
    {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'},
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}, 
    {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'}, 
    {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'}};
byte rowPins[rows] = {ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6};               //connect to the row pinouts of the keypad
byte colPins[cols] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

int x1 = 0, y1 = 0, x2 = 0, y2 = 0; // start top left
bool s = true;    // start with led on
bool first_choice_set = false;
bool second_choice_set = false;
bool first_choice_sent = false;
bool second_choice_sent = false;
char first_choice = '\0';
char second_choice = '\0';
bool update_leds = false;

// Bounce debouncer = Bounce(); 


bool debounce(int pin, int desired_state, int button_hold_time) {
    if( digitalRead(pin) == desired_state ) {
        int start_time = millis();
        while( millis() < start_time + button_hold_time ) {
            if( digitalRead(pin) != desired_state ) {
                return false;
            }
        }
        // If we haven't yet returned false then return true
        return true;
    }
    return false;
}


// void send_key(char key) {
//     PI_SERIAL.println(key);
//     Serial.println(key);
//     // Keyboard.begin();
//     // Keyboard.print(first_choice);
//     // Keyboard.end();
// }


void light_up_button(char key, int round) {
    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            if (keys[row][col] == key)
            {
                if( round == 1 ) {
                    x1 = col;
                    y1 = row;
                }
                if( round == 2 ) {
                    x2 = col;
                    y2 = row;
                }
                break;
            }
        }
    }
    lmd.clear();
    if( round == 2 ) {
        lmd.setPixel(x2, y2, s);    
    }
    lmd.setPixel(x1, y1, s);
    // Flush framebuffer
    lmd.display();
}


void clearButtonLeds() {
    lmd.clear();
    lmd.display();
}


void send_choices() {
    Serial.println("Choices Requested");
    if( first_choice_set == true && first_choice_sent == false ) {
        Serial.print("Sent first choice: ");
        Serial.println(first_choice);
        Wire.write(first_choice);
        first_choice_sent = true;
    } else if( second_choice_set == true && first_choice_sent == true && second_choice_sent == false ) {
        Serial.print("Sent second choice: ");
        Serial.println(second_choice);
        Wire.write(second_choice);
        second_choice_sent = true;
    } else if( first_choice_sent == true && second_choice_sent == true ) {
        Serial.println("Reset Choices");
        first_choice_sent = false;
        second_choice_sent = false;
        first_choice_set = false;
        second_choice_set = false;
        Wire.write('\0');
    }
}


void set_pixel(int howMany) {
    int led_num, red, green, blue;
    Serial.print("Set pixel how many: ");
    Serial.print(howMany);
    Serial.print(" Available: ");
    Serial.println(Wire.available());
    // while(Wire.available() > 0){
    //     Serial.print("Read: ");
    //     Serial.println( Wire.read() );
    // }
    // while( Wire.available()) {
    //Read first 2 bytes off
    Wire.read();
    Wire.read();
    led_num = Wire.read();
    // Serial.print("led_num: ");
    // Serial.println(led_num);
    // Serial.print(" Available: ");
    // Serial.println(Wire.available());
    red = Wire.read();
    // Serial.print("red: ");
    // Serial.println(red);
    // Serial.print(" Available: ");
    // Serial.println(Wire.available());
    green = Wire.read();
    // Serial.print("green: ");
    // Serial.println(green);
    // Serial.print(" Available: ");
    // Serial.println(Wire.available());
    blue = Wire.read();
    // Serial.print("blue: ");
    // Serial.println(blue);
    // Serial.print(" Available: ");
    // Serial.println(Wire.available());
    strip.setPixelColor(led_num, strip.Color(red, green, blue));
    update_leds = true;
    // }
    // strip.show();
}


void setup()
{
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    Wire.begin(8);
    Wire.onReceive(set_pixel);
    Wire.onRequest(send_choices);
    Serial.begin(115200);
    lmd.setEnabled(true);
    lmd.setIntensity(0xF); // 0 = low, 10 = high
    clearButtonLeds();
}


void loop()
{
    if(update_leds == true) {
        strip.show();
        update_leds = false;
    }
    
    if( first_choice_set == false || second_choice_set == false) {
        
        char key = keypad.getKey();

        if (key != NO_KEY)
        {
            Serial.println("check key press");
            if( isLowerCase(key) && first_choice_set == false ) {
                Serial.println("first choice");
                first_choice = key;
                first_choice_set = true;
                light_up_button(key, 1);
                // send_key(first_choice);
            } else if( isUpperCase(key) && first_choice_set == true && second_choice_set == false ) {
                Serial.println("second choice");
                second_choice = key;
                second_choice_set = true;
                light_up_button(key, 2);
                // send_key(second_choice);
            }
            Serial.println(key);
        }
    }
}

