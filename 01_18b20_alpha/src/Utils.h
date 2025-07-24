#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <DallasTemperature.h>

// 오류 코드를 위한 Enum
enum ErrorCode
{
    SUCCESS,
    ERROR_SENSOR_NOT_FOUND,
    ERROR_INVALID_ID,
    ERROR_ID_IN_USE,
    ERROR_TIMEOUT,
    ERROR_WRITE_FAILED
};

// 로깅 함수
void log_error(const char *message);
void log_info(const char *message);
void log_debug(const char *message);

// 유틸리티 함수
void addrToString(DeviceAddress deviceAddress, char *addrStr);

#endif // UTILS_H
