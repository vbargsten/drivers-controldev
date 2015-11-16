#include "GraupnerMC20.hpp"
#include <iostream>
#include <assert.h>
#define CRC_POLYNOME 0x1021

using namespace controldev;
GraupnerMC20::GraupnerMC20() : iodrivers_base::Driver(512, true)
{
}
//TODO openSerial(device, 115200)
//
bool GraupnerMC20::getReading(MC20Output *out){
    assert(isValid());
    int buffersize = 512;
    uint8_t buffer[buffersize];
    base::Time timeout = base::Time().fromSeconds(10);
    int len = readPacket(buffer, buffersize, timeout);
    if(len == 37){
        for(int i = 0; i < buffer[2]; i++){
           out->channel[i] = buffer[i*2+1]<<8 | buffer[i*2+2];
        }
        return true;
    } else {
        std::cout << len << std::endl;
    }

    return false;
}


int GraupnerMC20::extractPacket(uint8_t const* buffer, size_t buffer_size) const{
    size_t i = 0;
    for(i = 0; i < buffer_size; i++){
        if(buffer[i] == (uint8_t)0xA8){
            break;
        }
    }
    if(i>0){
        return -i;
    }
    if(buffer[1] != 0x01 && buffer[1] != 0x81){
        return -1;
    }

    if(buffer_size<3){
        return 0;
    }

    int length = buffer[2]*2+5;

    if((int)buffer_size < length){
        return 0;
    }

    uint16_t crc = (buffer[(buffer[2]+1)*2+1]<<8) + buffer[(buffer[2]+1)*2+2];

    if (crc == genCRC(buffer)){
        return length;
    }
        std::cout << "5"  << std::endl;
    return -1;
}

uint16_t GraupnerMC20::genCRC(uint8_t const* buffer) const{
    int size = (buffer[2])*2+3;
    uint16_t crc = 0;

    for(int i=0; i<size; i++){
        crc = CRC(crc, buffer[i]);
    }
    return crc;
}

uint16_t GraupnerMC20::CRC(uint16_t crc, uint8_t value) const{
    uint8_t i;

    crc = crc ^ ((int16_t)value)<<8;

    for(i=0; i<8; i++){
        if(crc & 0x8000){
            crc = (crc << 1) ^ CRC_POLYNOME;
        } else{
            crc = (crc << 1);
        }
    }
    return crc;

}