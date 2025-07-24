#include "Utils.h"

void log_error(const char *message)
{
    Serial.print("[ERROR] ");
    Serial.println(message);
}

void log_info(const char *message)
{
    Serial.print("[INFO] ");
    Serial.println(message);
}

void log_debug(const char *message)
{
    Serial.print("[DEBUG] ");
    Serial.println(message);
}

void addrToString(DeviceAddress deviceAddress, char *addrStr)
{
    int idx = 0;
    for (uint8_t j = 0; j < 8; j++)
    {
        sprintf(&addrStr[idx], "%02X", deviceAddress[j]);
        idx += 2;
        if ((j % 2 == 1) && (j != 7))
        {
            addrStr[idx++] = ':';
        }
    }
    addrStr[idx] = '\0';
}
