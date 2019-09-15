#ifndef LOG_H
#define LOG_H
#define LCD

#include <Arduino.h>
#ifdef LCD
#include "Adafruit_LiquidCrystal.h"
#endif

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__


class Log
{
private:
    uint8_t lcd_latch_pin;
    uint8_t lcd_clock_pin;
    uint8_t lcd_data_pin;
    unsigned long heartbeat_interval;
    bool heartbeat_on;
    int heartbeat_durration;
    unsigned long heartbeat_on_at;
    unsigned long heartbeat_off_at;
    #ifdef LCD
    Adafruit_LiquidCrystal lcd;
    #endif
    int current_row;
    int max_row;
    int message_num;
 
public:

    // Date(int year, int month, int day)
    // {
    //     setDate(year, month, day);
    // }
    // Default Contructor
    Log();

    #ifdef LCD
    Log(Adafruit_LiquidCrystal lcd);
    #else
    Log(void* lcd_void);
    #endif

    // void setDate(int year, int month, int day)
    // {
    //     m_year = year;
    //     m_month = month;
    //     m_day = day;
    // }
 
    // int getYear() { return m_year; }
    // int getMonth() { return m_month; }
    // int getDay()  { return m_day; }

    int freeMemory();

    void check_heartbeat() ;


    void print_log_pre(bool force, bool heartbeat);
    void print_log(char* msg, bool force, bool heartbeat);
    void print_log(const char* msg, bool force, bool heartbeat);
    void print_log(int msg, bool force, bool heartbeat);
    void print_log(byte msg, bool force, bool heartbeat);
    void heartbeat_log(const char* log_msg, bool force);
    void heartbeat_log(char* log_msg, bool force);
    void heartbeat_log(int log_msg, bool force) ;
    void heartbeat_log(byte log_msg, bool force);
    void heartbeat_log(char log_msg, bool force);
    void heartbeat_log(const char* msg);
    void heartbeat_log(char* msg);
    void heartbeat_log(char msg);
    void heartbeat_log(int msg);
    void heartbeat_log(byte msg);


    void print_lcd_pre();
    void print_lcd(const char*& msg, bool new_line);
    void print_lcd(char* msg, bool new_line);
    void print_lcd(char msg, bool new_line);
    void print_lcd(int msg, bool new_line);
    void print_lcd(byte msg, bool new_line);
    void lcd_log(const char*& log_msg, bool new_line = true);
    void lcd_log(char* log_msg, bool new_line = true);
    void lcd_log(int log_msg, bool new_line = true);
    void lcd_log(byte log_msg, bool new_line = true);
    void lcd_log(char log_msg, bool new_line = true);
    // void lcd_log(char* log_msg);
    // void lcd_log(int log_msg);
    // void lcd_log(byte log_msg);
    // void lcd_log(char log_msg);

};

#endif