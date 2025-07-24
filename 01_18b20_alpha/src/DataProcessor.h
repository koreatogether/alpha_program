#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include "DS18B20_Sensor.h"
#include "config.h"

// 센서 정보 구조체
struct SensorInfo
{
    int deviceId;
    DeviceAddress addr;
    float temp;
    bool valid;
};

class DataProcessor
{
public:
    DataProcessor(DS18B20_Sensor *sensor);
    void printSensorTable();
    void printInvalidSensorTable();
    void printAvailableIds();
    bool hasInvalidSensors();

private:
    DS18B20_Sensor *_sensor;
};

#endif // DATA_PROCESSOR_H
