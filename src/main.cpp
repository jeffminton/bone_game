#include <Arduino.h>
#include <Keypad.h>
#include <LEDMatrixDriver.hpp>
#include <stdarg.h>

#include "i2c.h"
#include "log.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif  

// #include <Wire.h>

// Define a TEST_BUTTON_SETUP to declare a limited number of buttons to test
// #define TEST_BUTTON_SETUP 1

// Define a setup where buttons are set automativally for testing without buttons plugged in
// #define NO_BUTTON_TEST 1



#define LED_PIN 4

#define PAD_1_ROW_1 7
#define PAD_1_ROW_2 8
#define PAD_1_ROW_3 9
#define PAD_2_ROW_1 10
#define PAD_2_ROW_2 11
#define PAD_2_ROW_3 12
#define COL_1 13
#define COL_2 14
#define COL_3 15
#define COL_4 16
#define COL_5 17
#define COL_6 18
#define COL_7 19
#define COL_8 20


// Constants
const int LED_COUNT = 54;

const int NON_CORRECT_INDICATOR_LED_COUNT = 48;

const uint8_t LCD_CLOCK_PIN = 21;
const uint8_t LCD_DATA_PIN = 23;
const uint8_t LCD_LATCH_PIN = 24;

// LED Matrix chip select pin
const uint8_t LEDMATRIX_CS_PIN = 0;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7
const int LEDMATRIX_WIDTH = 8;
const int LEDMATRIX_HEIGHT = 6;
const int LEDMATRIX_SEGMENTS = 1;

const int thinking_led_rows = 12;
const int thinking_led_cols = 4;
int thinking_leds[thinking_led_rows][thinking_led_cols] = {
    {0, 12, 24, 36},
    {1, 13, 25, 37},
    {2, 14, 26, 38},
    {3, 15, 27, 39},
    {4, 16, 28, 40},
    {5, 17, 29, 41},
    {6, 18, 30, 42},
    {7, 19, 31, 43},
    {8, 20, 32, 44},
    {9, 21, 33, 45},
    {10, 22, 34 ,46},
    {11, 23, 35, 47}
};

unsigned long thinking_times[] = {
     061,  338,  645,
     795, 1075, 1371,
    1522, 1810, 2095,
    2246, 2615, 3051,
    3347, 3541
};

unsigned long thinking_time_end = 3541;

// Define keypad constants
#ifndef TEST_BUTTON_SETUP
const int rows = 6; //6 rows
const int cols = 8; //8 columns
const byte pad_1_rows = 3; //3 rows
const byte pad_1_cols = 8; //8 columns
const byte pad_2_rows = 3; //3 rows
const byte pad_2_cols = 8; //8 columns
#else
const int rows = 6; //6 rows
const int cols = 8; //8 columns
const byte pad_1_rows = 3; //3 rows
const byte pad_1_cols = 2; //8 columns
const byte pad_2_rows = 3; //3 rows
const byte pad_2_cols = 2; //8 columns
#endif

// Define keypad keys
char keys[rows][cols] = {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'},
    {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'},
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'},
    {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'},
    {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'}};



// Define keypad 1 keys
#ifndef TEST_BUTTON_SETUP
char pad_1_keys[pad_1_rows][pad_1_cols] = {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'},
    {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'}};
#else
char pad_1_keys[pad_1_rows][pad_1_cols] = {
    {'a', 'b'},
    {'i', 'j'},
    {'q', 'r'}};
#endif

// Define keypad 2 keys
#ifndef TEST_BUTTON_SETUP
char pad_2_keys[pad_2_rows][pad_2_cols] = {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'},
    {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'},
    {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'}};
#else
char pad_2_keys[pad_2_rows][pad_2_cols] = {
    {'A', 'B'},
    {'I', 'J'},
    {'Q', 'R'}};
#endif

#ifndef TEST_BUTTON_SETUP
// Define keypad 1 pins
byte pad_1_rowPins[pad_1_rows] = {PAD_1_ROW_1, PAD_1_ROW_2, PAD_1_ROW_3};                  //connect to the row pinouts of the keypad
byte pad_1_colPins[pad_1_cols] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8}; //connect to the column pinouts of the keypad

// Define keypad 2 pins
byte pad_2_rowPins[pad_2_rows] = {PAD_2_ROW_1, PAD_2_ROW_2, PAD_2_ROW_3};                  //connect to the row pinouts of the keypad
byte pad_2_colPins[pad_2_cols] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8}; //connect to the column pinouts of the keypad
#else
// Define keypad 1 pins
byte pad_1_rowPins[pad_1_rows] = {PAD_1_ROW_1, PAD_1_ROW_2, PAD_1_ROW_3};                  //connect to the row pinouts of the keypad
byte pad_1_colPins[pad_1_cols] = {COL_1, COL_2}; //connect to the column pinouts of the keypad

// Define keypad 2 pins
byte pad_2_rowPins[pad_2_rows] = {PAD_2_ROW_1, PAD_2_ROW_2, PAD_2_ROW_3};                  //connect to the row pinouts of the keypad
byte pad_2_colPins[pad_2_cols] = {COL_1, COL_2}; //connect to the column pinouts of the keypad
#endif





// Command ENUM
enum commands
{
    set_led,
    clear_then_set_led,
    clear_strip,
    set_multiple_leds,
    reset_game,
    led_test,
    button_test_on,
    button_test_off,
    reset_teensy,
    heartbeat,
    set_send_log,
    set_random_leds,
    start_thinking_leds,
    end_thinking_leds
};

// Heartbeat message enum
enum heartbeat_messages {
    waiting_for_test_choice,
    waiting_for_first_choice,
    waiting_for_second_choice,
    lighting_pressed_button,
    lighting_led,
    sent_test_choice,
    sent_first_choice,
    sent_second_choce,
    sent_heartbeat,
    startup
};



// Global vars
// Message array
char data_msg[21];

// Light active array
bool lights[rows][cols] = {
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false}};

// Selection stat vars
bool first_choice_set = false;
bool second_choice_set = false;
bool first_choice_sent = false;
bool second_choice_sent = false;
bool test_choice_set = false;
bool test_choice_sent = false;
char first_choice = '0';
char second_choice = '0';
char test_choice = '0';

bool update_leds = false;
bool button_test = false;
bool clear_key_list = false;
bool trigger_thinking_leds = false;

bool keys_in = false;
bool send_heartbeat = false;

byte heartbeat_message = startup;


// Global object defs
#ifdef LCD
Adafruit_LiquidCrystal lcd;
#endif

// Log object
Log out_log;

// I2C Object
I2C i2c;

// Neopixel strip
Adafruit_NeoPixel strip;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

// Keypad defs
Keypad pad_1_keypad = Keypad(makeKeymap(pad_1_keys), pad_1_rowPins, pad_1_colPins, pad_1_rows, pad_1_cols);
Keypad pad_2_keypad = Keypad(makeKeymap(pad_2_keys), pad_2_rowPins, pad_2_colPins, pad_2_rows, pad_2_cols);


//declare reset function @ address 0
void (*resetFunc)(void) = 0; 


bool int_in_array(int value, int* array, int items_in_array) {
    for (int i = 0; i < items_in_array; i++) {
        if (value == array[i]) {
            return true;
        }
    }

    return false;
}


bool int_near_array_value(int value, int* array, int items_in_array, int close_threshold) {
    int low_threshold, high_threshold;

    for (int i = 0; i < items_in_array; i++) {
        low_threshold = array[i] - close_threshold;
        high_threshold = array[i] + close_threshold;
        if (value < low_threshold || value > high_threshold) {
            return true;
        }
    }

    return false;
}


void light_up_button(char key, int round)
{
    lmd.clear();
    sprintf(data_msg, "light_up_button %c", key);
    out_log.heartbeat_log(data_msg);
    // out_log.lcd_log("lgt btn:");
    // out_log.lcd_log(key, false);
    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            if (keys[row][col] == key)
            {
                lights[row][col] = true;
            }
            if (lights[row][col] == true)
            {
                lmd.setPixel(col, row, true);
                heartbeat_message = lighting_pressed_button;
            }
            
        }
    }
    lmd.display();
}

void clear_button_leds()
{
    sprintf(data_msg, "clear_button_leds");
    out_log.heartbeat_log(data_msg, true);
    // out_log.lcd_log("clear_button_leds", true);
    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            lights[row][col] = false;
        }
    }
    lmd.clear();
    lmd.display();
}

void light_all_button_leds()
{
    sprintf(data_msg, "light_all_button_leds");
    out_log.heartbeat_log(data_msg, true);
    // out_log.lcd_log("light_all_button_leds");
    lmd.clear();
    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            lmd.setPixel(col, row, true);
        }
    }
    lmd.display();
}

void send_choices()
{
    // int bytes_sent = 0;
    // sprintf(data_msg, "Choices Requested");
    // out_log.heartbeat_log(data_msg);

    if (send_heartbeat == true)
    {
        // sprintf(data_msg, "Snd hb:%d", heartbeat_message);
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        // bytes_sent = i2c.write_data(heartbeat_message);
        i2c.write_data(heartbeat_message);
        // sprintf(data_msg, " b:%d", bytes_sent);
        // out_log.lcd_log(data_msg, false);
        send_heartbeat = false;
        heartbeat_message = sent_heartbeat;
    }
    // else if (button_test == true && test_choice_set == true)
    // {
    //     // sprintf(data_msg, "Send test button:%c", test_choice);
    //     // out_log.heartbeat_log(data_msg, true);
    //     i2c.write_data(test_choice);
    //     test_choice_set = false;
    //     light_up_button(test_choice, 1);
    //     heartbeat_message = sent_test_choice;
    // }
    else if (first_choice_set == true && first_choice_sent == false)
    {
        // sprintf(data_msg, "Snd fst key:%c", first_choice);
        // out_log.heartbeat_log(data_msg);
        // out_log.lcd_log(data_msg);
        i2c.write_data(first_choice);
        first_choice_sent = true;
        light_up_button(first_choice, 1);
        heartbeat_message = sent_first_choice;
    }
    else if (second_choice_set == true && first_choice_sent == true && second_choice_sent == false)
    {
        // sprintf(data_msg, "Snd sec key:%c", second_choice);
        // out_log.heartbeat_log(data_msg);
        // out_log.lcd_log(data_msg);
        i2c.write_data(second_choice);
        second_choice_sent = true;
        light_up_button(second_choice, 2);
        heartbeat_message = sent_second_choce;
    }
}

void clear_pixels()
{
    // sprintf(data_msg, "clear_pixels");
    // out_log.heartbeat_log(data_msg, true);
    // out_log.lcd_log("clear_pixels");
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    update_leds = true;
}

void light_all_pixels()
{
    sprintf(data_msg, "light_all_pixels");
    out_log.heartbeat_log(data_msg, true);
    // out_log.lcd_log("light_all_pixels");
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(i, strip.Color(255, 255, 255));
    }
    update_leds = true;
}

void set_pixels_from_wire()
{
    // sprintf(data_msg, "set_pixels_from_wire");
    // out_log.heartbeat_log(data_msg, true);
    // out_log.lcd_log("set_pixels_from_wire");
    int led_num, red, green, blue;
    led_num = Wire.read();
    red = Wire.read();
    green = Wire.read();
    blue = Wire.read();
    strip.setPixelColor(led_num, strip.Color(red, green, blue));
    update_leds = true;
    heartbeat_message = lighting_led;
}


void play_thinking_leds() {
    // Get the current time to compare
    // int log_line = 0;
    sprintf(data_msg, "think");
    out_log.lcd_log(data_msg);
    unsigned long start_time = millis();
    int row_num = thinking_led_rows - 1, col_1 = 0, col_2 = 2;
    int prev_row_num = row_num, prev_col_1 = col_1, prev_col_2 = col_2;
    bool down = false;
    unsigned long current_time = millis();
    int red = 0, green = 0, blue = 255;
    int next_processing_message_time = 1000;

    // Run the LEDs up the left
    sprintf(data_msg, "loop start");
    out_log.lcd_log(data_msg);
    while( ((start_time + thinking_time_end) > current_time) && trigger_thinking_leds == true ) {
        strip.setPixelColor(thinking_leds[prev_row_num][prev_col_1], strip.Color(0, 0, 0));
        strip.setPixelColor(thinking_leds[prev_row_num][prev_col_2], strip.Color(0, 0, 0));
        
        strip.setPixelColor(thinking_leds[row_num][col_1], strip.Color(red, green, blue));
        strip.setPixelColor(thinking_leds[row_num][col_2], strip.Color(red, green, blue));
        // // sprintf(data_msg, "t[%d, %d-%d]", row_num, col_1, col_2);
        // // out_log.lcd_log(data_msg);
        if((current_time - start_time) >= next_processing_message_time){
            sprintf(data_msg, "in think");
            out_log.lcd_log(data_msg);
            next_processing_message_time = next_processing_message_time + 1000;
        }

        strip.show();

        prev_row_num = row_num;
        prev_col_1 = col_1;
        prev_col_2 = col_2;
        if( down == true && (row_num + 1) < thinking_led_rows ) {
            // Going down on the right and not at the last row yet
            // Increment row
            row_num++;
        } else if ( down == true && (row_num + 1) >= thinking_led_rows) {
            // Going down on the right and at the last row. 
            // Switch to the other columns and go up
            down = false;
            // prev_col_1 = col_1;
            // prev_col_2 = col_2;
            col_1--;
            col_2--;
        } else if( down == false && row_num > 0) {
            // Going up on the left and not at the first row
            // Decrement row number
            row_num --;
        } else if( down == false && row_num == 0 ) {
            // Going up on the right and at the first row
            // Switch to the other coluns and go down
            down = true;
            // prev_col_1 = col_1;
            // prev_col_2 = col_2;
            col_1++;
            col_2++;
        }

        current_time = millis();

        delay(20);
    }
    sprintf(data_msg, "loop done");
    out_log.lcd_log(data_msg);
}


void read_command(int howMany)
{
    int command, set_count;
    // Read first 2 bytes off
    // byte1 = Wire.read();
    // byte2 = Wire.read();
    Wire.read();
    Wire.read();
    // Read command
    command = Wire.read();

    switch (command)
    {
    case clear_strip:
        // sprintf(data_msg, "CMD: clr_strp");
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        clear_pixels();
        break;
    case clear_then_set_led:
        // sprintf(data_msg, "CMD: clr_set_led");
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        clear_pixels();
        set_pixels_from_wire();
        break;
    case set_led:
        // sprintf(data_msg, "CMD: set_led");
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        set_pixels_from_wire();
        break;
    case set_multiple_leds:
        // sprintf(data_msg, "CMD: set_mult_leds");
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        set_count = Wire.read();
        for (int i = 0; i < set_count; i++)
        {
            set_pixels_from_wire();
        }
        break;
    case reset_game:
        sprintf(data_msg, "CMD: rst_game");
        out_log.heartbeat_log(data_msg, true);
        out_log.lcd_log(data_msg);
        first_choice = '0';
        second_choice = '0';
        first_choice_sent = false;
        second_choice_sent = false;
        first_choice_set = false;
        second_choice_set = false;
        clear_key_list = true;
        clear_pixels();
        clear_button_leds();
        break;
    case led_test:
        // sprintf(data_msg, "CMD: led_test");
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        light_all_button_leds();
        light_all_pixels();
        break;
    // case button_test_on:
    //     // sprintf(data_msg, "CMD: btn_test_on");
    //     // out_log.heartbeat_log(data_msg, true);
    //     // out_log.lcd_log(data_msg);
    //     button_test = true;
    //     break;
    // case button_test_off:
    //     // sprintf(data_msg, "CMD: btn_test_off");
    //     // out_log.heartbeat_log(data_msg, true);
    //     // out_log.lcd_log(data_msg);
    //     button_test = false;
    //     break;
    case reset_teensy:
        // sprintf(data_msg, "CMD: rst_teensy");
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        resetFunc(); //call reset
        break;
    case heartbeat:
        // sprintf(data_msg, "CMD: hb");
        // out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        send_heartbeat = true;
        break;
    case start_thinking_leds:
        sprintf(data_msg, "CMD: think");
        // out_log.heartbeat_log(data_msg, true);
        out_log.lcd_log(data_msg);
        trigger_thinking_leds = true; //call reset
        break;
    case end_thinking_leds:
        sprintf(data_msg, "CMD: nothink");
        // out_log.heartbeat_log(data_msg, true);
        out_log.lcd_log(data_msg);
        trigger_thinking_leds = false; //call reset
        break;
    // case set_send_log:
    //     out_log.heartbeat_log("Command: set_send_log", true);
    //     send_log = true;
    //     break;
    }

    // }
    // strip.show();
}


void key_listener_1(char key) {
    sprintf(data_msg, "Pad 1 Pressed %c", key);
    // out_log.lcd_log(data_msg);
    // if (button_test == true) {
    //     test_choice = key;
    //     test_choice_set = true;

    //     if(test_choice_set == false) {
    //         heartbeat_message = waiting_for_test_choice;
    //     }
    // } else 
    if (first_choice_set == false) {
        sprintf(data_msg, "first choice:%c", key);
        out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        first_choice = key;
        first_choice_set = true;
        out_log.heartbeat_log(key);
    }
}


void key_listener_2(char key) {
    sprintf(data_msg, "Pad 2 Pressed %c", key);
    // out_log.lcd_log(data_msg);
    // if (button_test == true) {
    //     test_choice = key;
    //     test_choice_set = true;

    //     if(test_choice_set == false) {
    //         heartbeat_message = waiting_for_test_choice;
    //     }
    // } else 
    if (first_choice_set == true && second_choice_set == false) {
        sprintf(data_msg, "second choice:%c", key);
        out_log.heartbeat_log(data_msg, true);
        // out_log.lcd_log(data_msg);
        second_choice = key;
        second_choice_set = true;
        out_log.heartbeat_log(key);
    }
}


void setup()
{
    Serial.begin(9600);

    #ifdef LCD
    lcd = Adafruit_LiquidCrystal(LCD_DATA_PIN, LCD_CLOCK_PIN, LCD_LATCH_PIN);
    #endif

    #ifdef LCD
    out_log = Log(lcd);
    #else
    out_log = Log(NULL);
    #endif

    sprintf(data_msg, "Teensy Init");
    out_log.heartbeat_log(data_msg, false);
    out_log.lcd_log(data_msg);

    strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show(); // Initialize all pixels to 'off'

    i2c = I2C(8, send_choices, read_command);

    lmd.setEnabled(true);
    lmd.setIntensity(0xF); // 0 = low, 10 = high
    clear_button_leds();
    
    pad_1_keypad.addEventListener(key_listener_1);
    pad_2_keypad.addEventListener(key_listener_2);

    randomSeed(analogRead(0));
}

void loop()
{
    if (update_leds == true)
    {
        strip.show();
        update_leds = false;
    }

    if( clear_key_list == true) {
        pad_1_keypad.clearList();
        pad_2_keypad.clearList();
        clear_key_list = false;
    }


    if (trigger_thinking_leds == true) {
        play_thinking_leds();
        trigger_thinking_leds = false;
    }

    #ifdef NO_BUTTON_TEST
        key_listener_1('a');
        key_listener_2('A');
    #else
        pad_1_keypad.getKeys();
        pad_2_keypad.getKeys();
    #endif

    // if (button_test == true)
    // {
    //     // log_button_buffer();
    //     sprintf(data_msg, "Get Pad 1 Keys");
    //     out_log.heartbeat_log(data_msg);
    //     keys_in = pad_1_keypad.getKeys();
    //     // interrupts();

    //     if (keys_in && test_choice_set == false)
    //     {
    //         for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    //         {
    //             if (pad_1_keypad.key[i].stateChanged) // Only find keys that have changed state.
    //             {
    //                 if (pad_1_keypad.key[i].kstate == PRESSED)
    //                 {
    //                     test_choice = pad_1_keypad.key[i].kchar;
    //                     test_choice_set = true;
    //                     // light_up_button(test_choice, 1);
    //                     break;
    //                 }
    //             }
    //         }
    //     }

    //     sprintf(data_msg, "Get Pad 2 Keys");
    //     out_log.heartbeat_log(data_msg);
    //     keys_in = pad_2_keypad.getKeys();
    //     // interrupts();

    //     if (keys_in && test_choice_set == false)
    //     {
    //         for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    //         {
    //             if (pad_2_keypad.key[i].stateChanged) // Only find keys that have changed state.
    //             {
    //                 if (pad_2_keypad.key[i].kstate == PRESSED)
    //                 {
    //                     test_choice = pad_1_keypad.key[i].kchar;
    //                     test_choice_set = true;
    //                     // light_up_button(test_choice, 1);
    //                     break;
    //                 }
    //             }
    //         }
    //     }

    //     if(test_choice_set == false) {
    //         heartbeat_message = waiting_for_test_choice;
    //     }
    // }
    // else 
    if (first_choice_set == false || second_choice_set == false)
    {
        
        if (first_choice_set == false)
        {
            sprintf(data_msg, "Waiting For First Choice");
            out_log.heartbeat_log(data_msg);
            // out_log.lcd_log(data_msg);
            heartbeat_message = waiting_for_first_choice;
        }
        else if (first_choice_set == true && second_choice_set == false)
        {
            sprintf(data_msg, "first choice");
            out_log.heartbeat_log(data_msg);
            out_log.heartbeat_log(first_choice);
            // out_log.lcd_log(data_msg);
            sprintf(data_msg, "Waiting For Second Choice");
            out_log.heartbeat_log(data_msg);
            heartbeat_message = waiting_for_second_choice;
        }
        else if (first_choice_set == true && second_choice_set == true)
        {
            sprintf(data_msg, "First Choice");
            out_log.heartbeat_log(data_msg);
            out_log.heartbeat_log(first_choice);
            // out_log.lcd_log(data_msg);
            sprintf(data_msg, "Second Choice");
            out_log.heartbeat_log(data_msg);
            out_log.heartbeat_log(second_choice);
            // out_log.lcd_log(data_msg);
        }
    }

        // if (first_choice_set == false)
        // {
        //     // noInterrupts();
        //     keys_in = pad_1_keypad.getKeys();
        //     // interrupts();

        //     bool key_pressed = false;

        //     if (keys_in)
        //     {
        //         // String key_list = String("Keys Pressed: ");
        //         for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
        //         {
        //             if (pad_1_keypad.key[i].stateChanged) // Only find keys that have changed state.
        //             {
        //                 if (pad_1_keypad.key[i].kstate == PRESSED)
        //                 {
        //                     key_pressed = true;
        //                     if (isLowerCase(pad_1_keypad.key[i].kchar) && first_choice_set == false)
        //                     {
        //                         sprintf(data_msg, "first choice");
        //                         out_log.heartbeat_log(data_msg, true);
                                
        //                         first_choice = pad_1_keypad.key[i].kchar;
        //                         first_choice_set = true;
        //                     }
        //                     out_log.heartbeat_log(pad_1_keypad.key[i].kchar);
        //                 }
        //             }
        //         }
        //         if (key_pressed == true)
        //         {
        //             // out_log.heartbeat_log(String(key_list), true);
        //             key_pressed = false;
        //         }
        //     }
        // } else if (first_choice_set == true && second_choice_set == false) {
        //     // noInterrupts();
        //     keys_in = pad_2_keypad.getKeys();
        //     // interrupts();

        //     bool key_pressed = false;

        //     if (keys_in)
        //     {
        //         // String key_list = String("Keys Pressed: ");
        //         for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
        //         {
        //             if (pad_2_keypad.key[i].stateChanged) // Only find keys that have changed state.
        //             {
        //                 if (pad_2_keypad.key[i].kstate == PRESSED)
        //                 {
        //                     key_pressed = true;
        //                     if (isUpperCase(pad_2_keypad.key[i].kchar) && first_choice_set == true && second_choice_set == false)
        //                     {
        //                         sprintf(data_msg, "second choice");
        //                         out_log.heartbeat_log(data_msg, true);
        //                         second_choice = pad_2_keypad.key[i].kchar;
        //                         second_choice_set = true;
        //                     }
        //                     out_log.heartbeat_log(pad_2_keypad.key[i].kchar);
        //                 }
        //             }
        //         }
        //         if (key_pressed == true)
        //         {
        //             // out_log.heartbeat_log(String(key_list), true);
        //             key_pressed = false;
        //         }
        //     }
        // }
    // }
}
