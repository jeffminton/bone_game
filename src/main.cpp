#include <Arduino.h>
#include <Keypad.h>
#include <LEDMatrixDriver.hpp>
#include <Wire.h>
#include <stdarg.h>

#include "log.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

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


#define LCD_CLK 21
const uint8_t lcd_clock_pin = 21;
#define LCD_DAT 23
const uint8_t lcd_data_pin = 23;
#define LCD_LAT 24
const uint8_t lcd_latch_pin = 24;

Adafruit_LiquidCrystal lcd(lcd_data_pin, lcd_clock_pin, lcd_latch_pin);

const int LED_COUNT = 48;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Log out_log(lcd_data, lcd_clock, lcd_latch);

Log out_log(lcd);

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 0;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7
const int LEDMATRIX_WIDTH = 8;
const int LEDMATRIX_HEIGHT = 6;
const int LEDMATRIX_SEGMENTS = 1;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

const int rows = 6; //6 rows
const int cols = 8; //8 columns
const byte pad_1_rows = 3; //3 rows
const byte pad_1_cols = 8; //8 columns
const byte pad_2_rows = 3; //3 rows
const byte pad_2_cols = 8; //8 columns
char keys[rows][cols] = {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'},
    {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'},
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'},
    {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'},
    {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'}};

char pad_1_keys[pad_1_rows][pad_1_cols] = {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'},
    {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'}};

// char pad_1_keys[pad_1_rows][pad_1_cols] = {
//     {1, 2, 3, 4, 5, 6, 7, 8},
//     {9, 10, 11, 12, 13, 14, 15, 16},
//     {17, 18, 19, 20, 21, 22, 23, 24}};

char pad_2_keys[pad_2_rows][pad_2_cols] = {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'},
    {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'},
    {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'}};

bool lights[rows][cols] = {
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false}};


byte pad_1_rowPins[pad_1_rows] = {PAD_1_ROW_1, PAD_1_ROW_2, PAD_1_ROW_3};                  //connect to the row pinouts of the keypad
byte pad_1_colPins[pad_1_cols] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8}; //connect to the column pinouts of the keypad
Keypad pad_1_keypad = Keypad(makeKeymap(pad_1_keys), pad_1_rowPins, pad_1_colPins, pad_1_rows, pad_1_cols);

byte pad_2_rowPins[pad_2_rows] = {PAD_2_ROW_1, PAD_2_ROW_2, PAD_2_ROW_3};                  //connect to the row pinouts of the keypad
byte pad_2_colPins[pad_2_cols] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8}; //connect to the column pinouts of the keypad
Keypad pad_2_keypad = Keypad(makeKeymap(pad_2_keys), pad_2_rowPins, pad_2_colPins, pad_2_rows, pad_2_cols);

// int x1 = 0, y1 = 0, x2 = 0, y2 = 0; // start top left
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
// unsigned long last_heartbeat = 0;



bool keys_in = false;
bool send_heartbeat = false;
// bool send_log = false;

// String print_log_msg;
// const int msg_send_buffer_length = 32;
// byte msg_send_buffer[msg_send_buffer_length] = {'0'};

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
    set_send_log
};


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

byte heartbeat_message = startup;

void (*resetFunc)(void) = 0; //declare reset function @ address 0



void light_up_button(char key, int round)
{
    lmd.clear();
    const char *msg0 = "light_up_button";
    out_log.heartbeat_log(msg0);
    out_log.heartbeat_log(key);
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
    const char *msg1 = "clear_button_leds";
    out_log.heartbeat_log(msg1, true);
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
    const char *msg2 = "light_all_button_leds";
    out_log.heartbeat_log(msg2, true);
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
    const char *msg3 = "Choices Requested";
    out_log.heartbeat_log(msg3);

    if (send_heartbeat == true)
    {
        const char *msg4 = "Send heartbeat";
        out_log.heartbeat_log(msg4, true);
        out_log.heartbeat_log(heartbeat_message, true);
        const char *msg5 = "Snd hb:";
        out_log.lcd_log(msg5);
        out_log.lcd_log(heartbeat_message, false);
        Wire.write(heartbeat_message);
        send_heartbeat = false;
        heartbeat_message = sent_heartbeat;
    }
    else if (button_test == true && test_choice_set == true)
    {
        const char *msg6 = "Send test button: ";
        out_log.heartbeat_log(msg6, true);
        out_log.heartbeat_log(test_choice, true);
        Wire.write(test_choice);
        test_choice_set = false;
        light_up_button(test_choice, 1);
        heartbeat_message = sent_test_choice;
    }
    else if (first_choice_set == true && first_choice_sent == false)
    {
        const char *msg7 = "Send first choice: ";
        out_log.heartbeat_log(msg7);
        out_log.heartbeat_log(first_choice, true);
        const char *msg8 = "Snd fst key:";
        out_log.lcd_log(msg8);
        out_log.lcd_log(first_choice, false);
        Wire.write(first_choice);
        first_choice_sent = true;
        light_up_button(first_choice, 1);
        heartbeat_message = sent_first_choice;
    }
    else if (second_choice_set == true && first_choice_sent == true && second_choice_sent == false)
    {
        const char *msg9 = "Send second choice: ";
        out_log.heartbeat_log(msg9);
        out_log.heartbeat_log(second_choice, true);
        const char *msg10 = "Snd sec key:";
        out_log.lcd_log(msg10);
        out_log.lcd_log(second_choice, false);
        Wire.write(second_choice);
        second_choice_sent = true;
        light_up_button(second_choice, 2);
        heartbeat_message = sent_second_choce;
    }
}

void clear_pixels()
{
    const char *msg11 = "clear_pixels";
    out_log.heartbeat_log(msg11, true);
    // out_log.lcd_log("clear_pixels");
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    update_leds = true;
}

void light_all_pixels()
{
    const char *msg12 = "light_all_pixels";
    out_log.heartbeat_log(msg12, true);
    // out_log.lcd_log("light_all_pixels");
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(i, strip.Color(255, 255, 255));
    }
    update_leds = true;
}

void set_pixels_from_wire()
{
    const char *msg13 = "set_pixels_from_wire";
    out_log.heartbeat_log(msg13, true);
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

void read_command(int howMany)
{
    int command, set_count;
    // Read first 2 bytes off
    Wire.read();
    Wire.read();
    // Read command
    command = Wire.read();

    const char *msg14 = "CMD: clr_strp";
    const char *msg15 = "CMD: clr_set_led";
    const char *msg16 = "CMD: set_led";
    const char *msg17 = "CMD: set_mult_leds";
    const char *msg18 = "CMD: rst_game";
    const char *msg19 = "CMD: led_test";
    const char *msg20 = "CMD: btn_test_on";
    const char *msg21 = "CMD: btn_test_off";
    const char *msg22 = "CMD: rst_teensy";
    const char *msg23 = "CMD: hb";

    switch (command)
    {
    case clear_strip:
        out_log.heartbeat_log(msg14, true);
        out_log.lcd_log(msg14);
        clear_pixels();
        break;
    case clear_then_set_led:
        out_log.heartbeat_log(msg15, true);
        out_log.lcd_log(msg15);
        clear_pixels();
        set_pixels_from_wire();
        break;
    case set_led:
        out_log.heartbeat_log(msg16, true);
        out_log.lcd_log(msg16);
        set_pixels_from_wire();
        break;
    case set_multiple_leds:
        out_log.heartbeat_log(msg17, true);
        out_log.lcd_log(msg17);
        set_count = Wire.read();
        for (int i = 0; i < set_count; i++)
        {
            set_pixels_from_wire();
        }
        break;
    case reset_game:
        out_log.heartbeat_log(msg18, true);
        out_log.lcd_log(msg18);
        clear_pixels();
        clear_button_leds();
        first_choice_sent = false;
        second_choice_sent = false;
        first_choice_set = false;
        second_choice_set = false;
        break;
    case led_test:
        out_log.heartbeat_log(msg19, true);
        out_log.lcd_log(msg19);
        light_all_button_leds();
        light_all_pixels();
        break;
    case button_test_on:
        out_log.heartbeat_log(msg20, true);
        out_log.lcd_log(msg20);
        button_test = true;
        break;
    case button_test_off:
        out_log.heartbeat_log(msg21, true);
        out_log.lcd_log(msg21);
        button_test = false;
        break;
    case reset_teensy:
        out_log.heartbeat_log(msg22, true);
        out_log.lcd_log(msg22);
        resetFunc(); //call reset
        break;
    case heartbeat:
        out_log.heartbeat_log(msg23, true);
        out_log.lcd_log(msg23);
        send_heartbeat = true;
        break;
    // case set_send_log:
    //     out_log.heartbeat_log("Command: set_send_log", true);
    //     send_log = true;
    //     break;
    }

    // }
    // strip.show();
}


void setup()
{
    Serial.begin(9600);
    const char *msg24 = "Teensy Init";
    out_log.heartbeat_log(msg24, false);
    out_log.lcd_log(msg24);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    Wire.begin(8);
    Wire.onReceive(read_command);
    Wire.onRequest(send_choices);
    lmd.setEnabled(true);
    lmd.setIntensity(0xF); // 0 = low, 10 = high
    clear_button_leds();
    // clear_button_buffer();
}

void loop()
{
    if (update_leds == true)
    {
        strip.show();
        update_leds = false;
    }

    if (button_test == true)
    {
        // log_button_buffer();
        const char *msg25 = "Get Pad 1 Keys";
        out_log.heartbeat_log(msg25);
        keys_in = pad_1_keypad.getKeys();
        // interrupts();

        if (keys_in && test_choice_set == false)
        {
            for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
            {
                if (pad_1_keypad.key[i].stateChanged) // Only find keys that have changed state.
                {
                    if (pad_1_keypad.key[i].kstate == PRESSED)
                    {
                        test_choice = pad_1_keypad.key[i].kchar;
                        test_choice_set = true;
                        // light_up_button(test_choice, 1);
                        break;
                    }
                }
            }
        }

        const char *msg26 = "Get Pad 2 Keys";
        out_log.heartbeat_log(msg26);
        keys_in = pad_2_keypad.getKeys();
        // interrupts();

        if (keys_in && test_choice_set == false)
        {
            for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
            {
                if (pad_2_keypad.key[i].stateChanged) // Only find keys that have changed state.
                {
                    if (pad_2_keypad.key[i].kstate == PRESSED)
                    {
                        test_choice = pad_1_keypad.key[i].kchar;
                        test_choice_set = true;
                        // light_up_button(test_choice, 1);
                        break;
                    }
                }
            }
        }

        if(test_choice_set == false) {
            heartbeat_message = waiting_for_test_choice;
        }
    }
    else if (first_choice_set == false || second_choice_set == false)
    {
        if (first_choice_set == false)
        {
            const char *msg27 = "Waiting For First Choice";
            out_log.heartbeat_log(msg27);
            heartbeat_message = waiting_for_first_choice;
        }
        else if (first_choice_set == true && second_choice_set == false)
        {
            const char *msg28 = "first choice";
            out_log.heartbeat_log(msg28);
            out_log.heartbeat_log(first_choice);
            const char *msg29 = "Waiting For Second Choice";
            out_log.heartbeat_log(msg29);
            heartbeat_message = waiting_for_second_choice;
        }
        else if (first_choice_set == true && second_choice_set == true)
        {
            const char *msg30 = "First Choice";
            out_log.heartbeat_log(msg30);
            out_log.heartbeat_log(first_choice);
            const char *msg31 = "Second Choice";
            out_log.heartbeat_log(msg31);
            out_log.heartbeat_log(second_choice);
        }

        if (first_choice_set == false)
        {
            // noInterrupts();
            keys_in = pad_1_keypad.getKeys();
            // interrupts();

            bool key_pressed = false;

            if (keys_in)
            {
                // String key_list = String("Keys Pressed: ");
                for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
                {
                    if (pad_1_keypad.key[i].stateChanged) // Only find keys that have changed state.
                    {
                        if (pad_1_keypad.key[i].kstate == PRESSED)
                        {
                            key_pressed = true;
                            if (isLowerCase(pad_1_keypad.key[i].kchar) && first_choice_set == false)
                            {
                                const char *msg32 = "first choice";
                                out_log.heartbeat_log(msg32, true);
                                
                                first_choice = pad_1_keypad.key[i].kchar;
                                first_choice_set = true;
                                // light_up_button(first_choice, 1);
                                // send_key(first_choice);
                                // out_log.lcd_log("frst key:");
                                // out_log.lcd_log(first_choice, false);
                            }
                            out_log.heartbeat_log(pad_1_keypad.key[i].kchar);
                        }
                    }
                }
                if (key_pressed == true)
                {
                    // out_log.heartbeat_log(String(key_list), true);
                    key_pressed = false;
                }
            }
        } else if (first_choice_set == true && second_choice_set == false) {
            // noInterrupts();
            keys_in = pad_2_keypad.getKeys();
            // interrupts();

            bool key_pressed = false;

            if (keys_in)
            {
                // String key_list = String("Keys Pressed: ");
                for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
                {
                    if (pad_2_keypad.key[i].stateChanged) // Only find keys that have changed state.
                    {
                        if (pad_2_keypad.key[i].kstate == PRESSED)
                        {
                            key_pressed = true;
                            if (isUpperCase(pad_2_keypad.key[i].kchar) && first_choice_set == true && second_choice_set == false)
                            {
                                const char *msg33 = "second choice";
                                out_log.heartbeat_log(msg33, true);
                                second_choice = pad_2_keypad.key[i].kchar;
                                second_choice_set = true;
                                // light_up_button(second_choice, 2);
                                // send_key(second_choice);
                                // out_log.lcd_log("sec key:");
                                // out_log.lcd_log(second_choice, true);
                            }
                            out_log.heartbeat_log(pad_2_keypad.key[i].kchar);
                        }
                    }
                }
                if (key_pressed == true)
                {
                    // out_log.heartbeat_log(String(key_list), true);
                    key_pressed = false;
                }
            }
        }
    }
}
