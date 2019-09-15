#ifndef I2C_H
#define I2C_H



#include <Wire.h>

class I2C
{
private:
    int i2c_address;
    TwoWire this_wire;
    
 
public:

    // Date(int year, int month, int day)
    // {
    //     setDate(year, month, day);
    // }

    I2C();
    I2C(int address, void (*)(void), void (*)(int));


    int write_data(uint8_t data);


};

#endif