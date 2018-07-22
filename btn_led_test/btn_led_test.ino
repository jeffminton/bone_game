#include <Arduino.h>
#include <Keypad.h>
#include <LEDMatrixDriver.hpp>
#include <Keyboard.h>
// #include <Bounce2.h>


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

#define RESTART_PIN 4

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
char first_choice = '\0';
char second_choice = '\0';

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


void send_key(char key) {
    Keyboard.begin();
    Keyboard.print(first_choice);
    Keyboard.end();
}


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


void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    // init the display
    lmd.setEnabled(true);
    lmd.setIntensity(0xF); // 0 = low, 10 = high
    clearButtonLeds();
    pinMode(RESTART_PIN, INPUT);

    // debouncer.attach(RESTART_PIN);
    // debouncer.interval(5); // interval in ms
}


void loop()
{
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
                send_key(first_choice);
            } else if( isUpperCase(key) && first_choice_set == true && second_choice_set == false ) {
                Serial.println("second choice");
                second_choice = key;
                second_choice_set = true;
                light_up_button(key, 2);
                send_key(second_choice);
            }
            Serial.println(key);
        }
    } else {
        if( debounce(RESTART_PIN, HIGH, 1000 ) ){
            Serial.println("Reset Button Pressed");
            first_choice_set = false;
            second_choice_set = false;
            clearButtonLeds();
        }
    }
}

