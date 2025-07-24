#include "DataProcessor.h"
#include "Utils.h"
#include <Arduino.h>

DataProcessor::DataProcessor(DS18B20_Sensor *sensor) : _sensor(sensor) {}

void DataProcessor::printSensorTable()
{
    SensorInfo sensorsTable[SENSOR_COUNT];
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensorsTable[i].valid = false;
        sensorsTable[i].deviceId = -1;
    }

    int found = _sensor->getDeviceCount();
    _sensor->requestTemperatures();

    for (int i = 0; i < found && i < SENSOR_COUNT; i++)
    {
        DeviceAddress addr;
        if (!_sensor->getAddress(addr, i))
        {
            continue;
        }
        int deviceId = _sensor->getUserDataByIndex(i);
        float temp = _sensor->getTempC(addr);
        if (deviceId >= 1 && deviceId <= SENSOR_COUNT)
        {
            int slotIndex = deviceId - 1;
            memcpy(sensorsTable[slotIndex].addr, addr, 8);
            sensorsTable[slotIndex].deviceId = deviceId;
            sensorsTable[slotIndex].temp = temp;
            sensorsTable[slotIndex].valid = true;
        }
    }

    char buf[128];
    Serial.println("---------------------------------------------------------------------------------------------");
    Serial.println("|  No |   Device ID   |        ADDRESS      |  Temp(℃) |  기준상한 | 초과온도 | 기준하한 | 초과온도 | 상태         |");
    Serial.println("---------------------------------------------------------------------------------------------");

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        if (!sensorsTable[i].valid)
        {
            snprintf(buf, sizeof(buf), "| %3d |     NONE      |        NONE         |   NONE   |   NONE  |   NONE  |   NONE  |   NONE  | 센서없음     |", i + 1);
            Serial.println(buf);
            continue;
        }
        char formattedAddr[32] = "";
        addrToString(sensorsTable[i].addr, formattedAddr);

        float temp = sensorsTable[i].temp;
        bool overUpper = temp > UPPER_TEMP_LIMIT;
        bool underLower = temp < LOWER_TEMP_LIMIT;
        const char *status = (overUpper || underLower) ? "점검 필요" : "이상없음";
        snprintf(buf, sizeof(buf),
                 "| %3d | %13d | %s | %8.2f | %8.2f | %7s | %8.2f | %7s | %-10s |",
                 i + 1,
                 sensorsTable[i].deviceId,
                 formattedAddr,
                 temp,
                 UPPER_TEMP_LIMIT,
                 overUpper ? "초과" : "+",
                 LOWER_TEMP_LIMIT,
                 underLower ? "초과" : "-",
                 status);
        Serial.println(buf);
    }
    Serial.println("---------------------------------------------------------------------------------------------");
}

void DataProcessor::printInvalidSensorTable()
{
    struct InvalidSensorInfo
    {
        int deviceId;
        DeviceAddress addr;
    };
    InvalidSensorInfo invalidList[SENSOR_COUNT];
    int found = _sensor->getDeviceCount();
    int invalidCount = 0;
    for (int i = 0; i < found; i++)
    {
        DeviceAddress addr;
        if (!_sensor->getAddress(addr, i))
            continue;
        int deviceId = _sensor->getUserDataByIndex(i);
        if (deviceId < 1 || deviceId > SENSOR_COUNT)
        {
            memcpy(invalidList[invalidCount].addr, addr, 8);
            invalidList[invalidCount].deviceId = deviceId;
            invalidCount++;
        }
    }

    for (int i = 0; i < invalidCount - 1; i++)
    {
        for (int j = i + 1; j < invalidCount; j++)
        {
            if (memcmp(invalidList[i].addr, invalidList[j].addr, 8) > 0)
            {
                InvalidSensorInfo tmp = invalidList[i];
                invalidList[i] = invalidList[j];
                invalidList[j] = tmp;
            }
        }
    }

    char buf[64];
    Serial.println("-------------------------------");
    Serial.println("|  Device ID  |    ADDRESS    |");
    Serial.println("-------------------------------");
    for (int i = 0; i < invalidCount; i++)
    {
        char addrStr[24] = "";
        addrToString(invalidList[i].addr, addrStr);
        snprintf(buf, sizeof(buf), "| %10d | %s |", invalidList[i].deviceId, addrStr);
        Serial.println(buf);
    }
    Serial.println("-------------------------------");
}

void DataProcessor::printAvailableIds()
{
    bool usedIds[SENSOR_COUNT + 1] = {false};
    int found = _sensor->getDeviceCount();
    for (int i = 0; i < found; i++)
    {
        DeviceAddress addr;
        _sensor->getAddress(addr, i);
        int id = _sensor->getUserData(addr);
        if (id >= 1 && id <= SENSOR_COUNT)
        {
            usedIds[id] = true;
        }
    }

    Serial.print(F("현재 사용 중인 ID: "));
    for (int i = 1; i <= SENSOR_COUNT; i++)
    {
        if (usedIds[i])
        {
            Serial.print(i);
            Serial.print(" ");
        }
    }
    Serial.println();
    Serial.print(F("사용 가능한 ID: "));
    for (int i = 1; i <= SENSOR_COUNT; i++)
    {
        if (!usedIds[i])
        {
            Serial.print(i);
            Serial.print(" ");
        }
    }
    Serial.println();
}

bool DataProcessor::hasInvalidSensors()
{
    int found = _sensor->getDeviceCount();
    for (int i = 0; i < found; i++)
    {
        DeviceAddress addr;
        if (!_sensor->getAddress(addr, i))
            continue;
        int deviceId = _sensor->getUserDataByIndex(i);
        if (deviceId < 1 || deviceId > SENSOR_COUNT)
        {
            return true;
        }
    }
    return false;
}
