#include "log.h"


// Default Constructor
Log::Log() {}

#ifdef LCD
Log::Log(Adafruit_LiquidCrystal lcd_ref) : lcd(lcd_ref) {
    heartbeat_interval = 1000;
    heartbeat_on = false;
    heartbeat_durration = 10;
    heartbeat_on_at = 0;
    heartbeat_off_at = 0;
    
    lcd = lcd_ref;
    // set up the LCD's number of rows and columns: 
    lcd.begin(20, 4);
    lcd.setBacklight(1);

    current_row = 0;
    max_row = 3;

    message_num = 0;
}
#else
Log::Log(void* lcd_void){
    heartbeat_interval = 1000;
    heartbeat_on = false;
    heartbeat_durration = 10;
    heartbeat_on_at = 0;
    heartbeat_off_at = 0;

    current_row = 0;
    max_row = 3;

    message_num = 0;
}
#endif


int Log::freeMemory()
{
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}



void Log::print_log_pre(bool force, bool heartbeat){
    // print_log_msg.remove(0);
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.print(", Free Mem: ");
    Serial.print(freeMemory());
    Serial.print(", MSG: ");
    if(force == true){
        Serial.print(", Forced: ");
    } else if(heartbeat == true){
        Serial.print(", Heartbeat: ");
    }
}

void Log::print_log(char* msg, bool force, bool heartbeat){
    print_log_pre(force, heartbeat);
    Serial.println(msg);
}

void Log::print_log(const char* msg, bool force, bool heartbeat){
    print_log_pre(force, heartbeat);
    Serial.println(msg);
}

void Log::print_log(int msg, bool force, bool heartbeat){
    print_log_pre(force, heartbeat);
    Serial.println(msg);
}

void Log::print_log(byte msg, bool force, bool heartbeat){
    print_log_pre(force, heartbeat);
    Serial.println(msg);
}



void Log::check_heartbeat() {
    if(heartbeat_on_at == 0) {
        // Serial.println('Heartbeat is None')
        heartbeat_on_at = millis() + heartbeat_interval;
        heartbeat_off_at = heartbeat_on_at + heartbeat_durration;
    }
    else if(millis() >= heartbeat_off_at) {
        heartbeat_on = false;
        //At the end of the pi heartbeat interval get the teensy logs
        heartbeat_on_at = millis() + heartbeat_interval;
        heartbeat_off_at = heartbeat_on_at + heartbeat_durration;
        // Serial.println('heartbeat off')
    }
    else if(millis() >= heartbeat_on_at) {
        // Serial.println('heartbeat on')
        heartbeat_on = true;
        // print_log("Heartbeat: ".concat(log_msg));
    }
}


void Log::heartbeat_log(char* log_msg, bool force) {
        // Serial.println('millis: %s, on at: %s, off at: %s' % (str(millis()), str(heartbeat_on_at), str(heartbeat_off_at)))
        if(force == true){
            print_log(log_msg, force, false);
        }
        check_heartbeat();
        if(heartbeat_on == true) {
            // print_log(log_msg, false, true);
        }
}


void Log::heartbeat_log(const char* log_msg, bool force) {
        // Serial.println('millis: %s, on at: %s, off at: %s' % (str(millis()), str(heartbeat_on_at), str(heartbeat_off_at)))
        if(force == true){
            print_log(log_msg, force, false);
        }
        check_heartbeat();
        if(heartbeat_on == true) {
            // print_log(log_msg, false, true);
        }
}


void Log::heartbeat_log(int log_msg, bool force) {
        // Serial.println('millis: %s, on at: %s, off at: %s' % (str(millis()), str(heartbeat_on_at), str(heartbeat_off_at)))
        if(force == true){
            print_log(log_msg, force, false);
        }
        check_heartbeat();
        if(heartbeat_on == true) {
            // print_log(log_msg, false, true);
        }
}

void Log::heartbeat_log(byte log_msg, bool force) {
        // Serial.println('millis: %s, on at: %s, off at: %s' % (str(millis()), str(heartbeat_on_at), str(heartbeat_off_at)))
        if(force == true){
            print_log(log_msg, force, false);
        }
        check_heartbeat();
        if(heartbeat_on == true) {
            // print_log(log_msg, false, true);
        }
}


void Log::heartbeat_log(char log_msg, bool force) {
    heartbeat_log(&log_msg, force);
}


void Log::heartbeat_log(char* msg){
    heartbeat_log(msg, false);
}

void Log::heartbeat_log(const char* msg){
    heartbeat_log(msg, false);
}

void Log::heartbeat_log(char msg){
    heartbeat_log(&msg, false);
}

void Log::heartbeat_log(int msg){
    heartbeat_log(msg, false);
}

void Log::heartbeat_log(byte msg){
    heartbeat_log(msg, false);
}




void Log::print_lcd_pre() {
    if( current_row > max_row ) {
        current_row = 0;
    }
    #ifdef LCD
    lcd.setCursor(0, current_row);
    lcd.print("                    ");
    lcd.setCursor(0, current_row);
    lcd.print(message_num);
    lcd.print(":");
    #endif
    message_num++;
}

void Log::print_lcd(const char*& msg, bool new_line){
    if(new_line == true) {
        print_lcd_pre();
    }
    #ifdef LCD
    lcd.print(msg);
    #endif
    current_row++;
}

void Log::print_lcd(char* msg, bool new_line){
    if(new_line == true) {
        print_lcd_pre();
    }
    #ifdef LCD
    lcd.print(msg);
    #endif
    current_row++;
}

void Log::print_lcd(char msg, bool new_line){
    if(new_line == true) {
        print_lcd_pre();
    }
    #ifdef LCD
    lcd.print(msg);
    #endif
    current_row++;
}

void Log::print_lcd(int msg, bool new_line){
    if(new_line == true) {
        print_lcd_pre();
    }
    #ifdef LCD
    lcd.print(msg);
    #endif
    current_row++;
}

void Log::print_lcd(byte msg, bool new_line){
    if(new_line == true) {
        print_lcd_pre();
    }
    #ifdef LCD
    lcd.print(msg);
    #endif
    current_row++;
}

void Log::lcd_log(const char*& log_msg, bool new_line) {
    print_lcd(log_msg, new_line);
}

void Log::lcd_log(char* log_msg, bool new_line) {
    print_lcd(log_msg, new_line);
}


void Log::lcd_log(int log_msg, bool new_line) {
    print_lcd(log_msg, new_line);
}

void Log::lcd_log(byte log_msg, bool new_line) {
    print_lcd(log_msg, new_line);
}


void Log::lcd_log(char log_msg, bool new_line) {
    print_lcd(log_msg, new_line);
}


