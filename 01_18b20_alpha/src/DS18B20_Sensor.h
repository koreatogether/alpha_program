#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

class DS18B20_Sensor
{
public:
    DS18B20_Sensor(OneWire *oneWire);
    void begin();
    int getDeviceCount();
    bool getAddress(DeviceAddress address, uint8_t index);
    void requestTemperatures();
    float getTempC(DeviceAddress address);
    int getUserData(DeviceAddress address);
    void setUserData(DeviceAddress address, int userData);
    int getUserDataByIndex(uint8_t index);
    bool isIdUsed(int id);

private:
    DallasTemperature _sensors;
};

#endif // DS18B20_SENSOR_H
