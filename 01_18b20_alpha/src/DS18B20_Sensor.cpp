#include "DS18B20_Sensor.h"

DS18B20_Sensor::DS18B20_Sensor(OneWire *oneWire) : _sensors(oneWire) {}

void DS18B20_Sensor::begin()
{
    _sensors.begin();
}

int DS18B20_Sensor::getDeviceCount()
{
    return _sensors.getDeviceCount();
}

bool DS18B20_Sensor::getAddress(DeviceAddress address, uint8_t index)
{
    return _sensors.getAddress(address, index);
}

void DS18B20_Sensor::requestTemperatures()
{
    _sensors.requestTemperatures();
}

float DS18B20_Sensor::getTempC(DeviceAddress address)
{
    return _sensors.getTempC(address);
}

int DS18B20_Sensor::getUserData(DeviceAddress address)
{
    // DallasTemperature 라이브러리의 getUserData 함수 사용
    return _sensors.getUserData(address);
}

void DS18B20_Sensor::setUserData(DeviceAddress address, int userData)
{
    // DallasTemperature 라이브러리의 setUserData 함수 사용
    _sensors.setUserData(address, userData);
}

int DS18B20_Sensor::getUserDataByIndex(uint8_t index)
{
    DeviceAddress addr;
    if (getAddress(addr, index))
    {
        return getUserData(addr);
    }
    return -1;
}

bool DS18B20_Sensor::isIdUsed(int id)
{
    if (id < 1 || id > SENSOR_COUNT)
        return true;

    int found = getDeviceCount();
    for (int i = 0; i < found; i++)
    {
        DeviceAddress addr;
        if (getAddress(addr, i))
        {
            if (getUserData(addr) == id)
            {
                return true;
            }
        }
    }
    return false;
}
