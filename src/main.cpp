#include <Arduino.h>
#include <Keypad.h>
#include <LEDMatrixDriver.hpp>
#include <Wire.h>

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

const int LED_COUNT = 8;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

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
const byte pad_1_rows = 3; //four rows
const byte pad_1_cols = 8; //three columns
const byte pad_2_rows = 3; //four rows
const byte pad_2_cols = 8; //three columns
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

int x1 = 0, y1 = 0, x2 = 0, y2 = 0; // start top left
bool first_choice_set = false;
bool second_choice_set = false;
bool first_choice_sent = false;
bool second_choice_sent = false;
char first_choice = '\0';
char second_choice = '\0';
bool update_leds = false;
bool button_test = false;
unsigned long last_heartbeat = 0, heartbeat_interval = 2000;
bool heartbeat_on = false;
bool keys_in = false;
bool send_heartbeat = false;

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
    heartbeat
};

void (*resetFunc)(void) = 0; //declare reset function @ address 0

bool debounce(int pin, int desired_state, int button_hold_time)
{
    if (digitalRead(pin) == desired_state)
    {
        unsigned int start_time = millis();
        while (millis() < start_time + button_hold_time)
        {
            if (digitalRead(pin) != desired_state)
            {
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

void light_up_button(char key, int round)
{
    lmd.clear();
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
            }
        }
    }

    // if( round == 2 ) {
    //     lmd.setPixel(x2, y2, s);
    // }
    // lmd.setPixel(x1, y1, s);
    // Flush framebuffer
    lmd.display();
}

void clear_button_leds()
{
    Serial.println("clear_button_leds");
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
    Serial.println("light_all_button_leds");
    for (int i = 0; i <= cols; i++)
    {
        lmd.setColumn(i, true);
    }
    lmd.display();
}

void send_choices()
{

    if (heartbeat_on)
    {
        Serial.println("Choices Requested");
    }

    if (button_test == true)
    {
        for (int col = 0; col < cols; col++)
        {
            for (int row = 0; row < rows; row++)
            {
                if (lights[row][col] == true)
                {
                    Wire.write(keys[row][col]);
                }
            }
        }
        Wire.write('0');
    }
    else if (first_choice_set == true && first_choice_sent == false)
    {
        Serial.print("Sent first choice: ");
        Serial.println(first_choice);
        Wire.write(first_choice);
        first_choice_sent = true;
    }
    else if (second_choice_set == true && first_choice_sent == true && second_choice_sent == false)
    {
        Serial.print("Sent second choice: ");
        Serial.println(second_choice);
        Wire.write(second_choice);
        second_choice_sent = true;
    }
    else if (send_heartbeat == true)
    {
        Serial.print("Send heartbeat: ");
        Wire.write('1');
        send_heartbeat = false;
    }
}

void clear_pixels()
{
    Serial.println("clear_pixels");
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    update_leds = true;
}

void light_all_pixels()
{
    Serial.println("light_all_pixels");
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(i, strip.Color(255, 255, 255));
    }
    update_leds = true;
}

void set_pixels_from_wire()
{
    Serial.println("set_pixels_from_wire");
    int led_num, red, green, blue;
    led_num = Wire.read();
    red = Wire.read();
    green = Wire.read();
    blue = Wire.read();
    strip.setPixelColor(led_num, strip.Color(red, green, blue));
    update_leds = true;
}

void read_command(int howMany)
{
    int command, set_count;
    // Read first 2 bytes off
    Wire.read();
    Wire.read();
    // Read command
    command = Wire.read();
    switch (command)
    {
    case clear_strip:
        clear_pixels();
        break;
    case clear_then_set_led:
        clear_pixels();
        set_pixels_from_wire();
        break;
    case set_led:
        set_pixels_from_wire();
        break;
    case set_multiple_leds:
        set_count = Wire.read();
        for (int i = 0; i < set_count; i++)
        {
            set_pixels_from_wire();
        }
        break;
    case reset_game:
        Serial.println("reset_game");
        clear_pixels();
        clear_button_leds();
        first_choice_sent = false;
        second_choice_sent = false;
        first_choice_set = false;
        second_choice_set = false;
        break;
    case led_test:
        Serial.println("led_test");
        light_all_pixels();
        break;
    case button_test_on:
        button_test = true;
        break;
    case button_test_off:
        button_test = false;
        break;
    case reset_teensy:
        resetFunc(); //call reset
        break;
    case heartbeat:
        send_heartbeat = true;
        break;
    }

    // }
    // strip.show();
}

void setup()
{
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    Wire.begin(8);
    Wire.onReceive(read_command);
    Wire.onRequest(send_choices);
    Serial.begin(115200);
    lmd.setEnabled(true);
    lmd.setIntensity(0xF); // 0 = low, 10 = high
    clear_button_leds();
}

void loop()
{
    if (last_heartbeat + heartbeat_interval < millis())
    {
        heartbeat_on = true;
        last_heartbeat = millis();
    }
    else
    {
        heartbeat_on = false;
    }

    if (update_leds == true)
    {
        strip.show();
        update_leds = false;
    }

    if (button_test == true)
    {
        Serial.println("button_test");
        keys_in = pad_1_keypad.getKeys();
        // interrupts();

        if (keys_in)
        {
            for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
            {
                if (pad_1_keypad.key[i].stateChanged) // Only find keys that have changed state.
                {
                    if (pad_1_keypad.key[i].kstate == PRESSED)
                    {
                        light_up_button(pad_1_keypad.key[i].kchar, 1);
                        // char key = pad_1_keypad.getKey();

                        // if (key != NO_KEY)
                        // {
                        //     light_up_button(key, 1);
                        //     // send_key(first_choice);
                        //     Serial.println(key);
                        // }
                    }
                }
            }
        }
    }
    else if (first_choice_set == false || second_choice_set == false)
    {
        if (heartbeat_on)
        {
            Serial.print("Time: ");
            Serial.print(last_heartbeat);
            Serial.print(", MSG: ");
            if (first_choice_set == false)
            {
                Serial.println("Waiting For First Choice");
            }
            else if (first_choice_set == true && second_choice_set == false)
            {
                Serial.print("First Choice: ");
                Serial.print(first_choice);
                Serial.println(", Waiting For Second Choice");
            }
            else if (first_choice_set == true && second_choice_set == true)
            {
                Serial.print("First Choice: ");
                Serial.print(first_choice);
                Serial.print(", Second Choice: ");
                Serial.println(second_choice);
            }
        }

        if (first_choice_set == false)
        {
            // char key = pad_1_keypad.getKey();
            // noInterrupts();
            keys_in = pad_1_keypad.getKeys();
            // interrupts();

            bool key_pressed = false;

            if (keys_in)
            {
                String key_list = String("Keys Pressed: ");
                for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
                {
                    if (pad_1_keypad.key[i].stateChanged) // Only find keys that have changed state.
                    {
                        if (pad_1_keypad.key[i].kstate == PRESSED)
                        {
                            key_pressed = true;
                            key_list.concat("Key index: ");
                            key_list.concat(i);
                            key_list.concat(", Key char: ");
                            key_list.concat(pad_1_keypad.key[i].kchar);
                            key_list.concat(String("; "));
                            if (isLowerCase(pad_1_keypad.key[i].kchar) && first_choice_set == false)
                            {
                                Serial.println("first choice");
                                first_choice = pad_1_keypad.key[i].kchar;
                                first_choice_set = true;
                                light_up_button(pad_1_keypad.key[i].kchar, 1);
                                // send_key(first_choice);
                            }
                            if (heartbeat_on)
                            {
                                Serial.println(pad_1_keypad.key[i].kchar);
                            }
                        }
                    }
                }
                if (key_pressed == true)
                {
                    Serial.println(key_list);
                    key_pressed = false;
                }
            }
        } else if (first_choice_set == true && second_choice_set == false) {
            // char key = pad_2_keypad.getKey();
            // noInterrupts();
            keys_in = pad_2_keypad.getKeys();
            // interrupts();

            bool key_pressed = false;

            if (keys_in)
            {
                String key_list = String("Keys Pressed: ");
                for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
                {
                    if (pad_2_keypad.key[i].stateChanged) // Only find keys that have changed state.
                    {
                        if (pad_2_keypad.key[i].kstate == PRESSED)
                        {
                            key_pressed = true;
                            key_list.concat("Key index: ");
                            key_list.concat(i);
                            key_list.concat(", Key char: ");
                            key_list.concat(pad_2_keypad.key[i].kchar);
                            key_list.concat(String("; "));
                            if (isUpperCase(pad_2_keypad.key[i].kchar) && first_choice_set == true && second_choice_set == false)
                            {
                                Serial.println("second choice");
                                second_choice = pad_2_keypad.key[i].kchar;
                                second_choice_set = true;
                                light_up_button(pad_2_keypad.key[i].kchar, 2);
                                // send_key(second_choice);
                            }
                            if (heartbeat_on)
                            {
                                Serial.println(pad_2_keypad.key[i].kchar);
                            }
                        }
                    }
                }
                if (key_pressed == true)
                {
                    Serial.println(key_list);
                    key_pressed = false;
                }
            }
        }

        // if (key != NO_KEY)
        // {
        //     Serial.println("check key press");

        // }
    }
}
