#include "i2c.h"


I2C::I2C() {}

//     I2C(int address, void (*)(void), void (*)(int));

I2C::I2C(int address, void (*onRequestFunc)(void), void (*onRecieveFunc)(int)) {
    i2c_address = address;

    this_wire = TwoWire();
    this_wire.begin(i2c_address);
    this_wire.onReceive(onRecieveFunc);
    this_wire.onRequest(onRequestFunc);
}


int I2C::write_data(uint8_t data){
    return this_wire.write(data);
}

