/*
배선도
DS18B20 온도 센서와 아두이노 연결
DS18B20 ------------- Arduino
VCC ------------- 5V
GND ------------- GND
DQ ------------- D2
*/

#include "config.h"
#include "DS18B20_Sensor.h"
#include "DataProcessor.h"
#include "Communicator.h"
#include "Utils.h"

// 애플리케이션 상태 정의
enum AppState
{
    NORMAL_OPERATION,
    MENU_ACTIVE,
    EDIT_INDIVIDUAL_ID,
    EDIT_SELECTIVE_ID,
    AUTO_ASSIGN_IDS,
    RESET_ALL_IDS
};

enum EditIdSubState
{
    EDIT_ID_NONE,
    EDIT_ID_SELECT_SENSOR,
    EDIT_ID_ENTER_NEW_ID,
    EDIT_SELECTIVE_GET_SENSORS,
    EDIT_SELECTIVE_CONFIRM,
    EDIT_SELECTIVE_ENTER_NEW_IDS
};

// 현재 애플리케이션 상태
AppState currentAppState = NORMAL_OPERATION;
EditIdSubState currentEditIdSubState = EDIT_ID_NONE;

// ID 변경을 위한 전역 변수
struct SensorInfoToChange
{
    DeviceAddress addr;
    int currentId;
    int index;
};
SensorInfoToChange sensorsToChangeArray[SENSOR_COUNT];
int sensorsToChangeCount = 0;
int selectedSensorIndex = -1;
unsigned long editIdTimeoutMillis = 0;

// 선택적 ID 변경을 위한 전역 변수
int selectedSensorIndices[SENSOR_COUNT];
int selectedSensorIndicesCount = 0;
int currentSelectiveIndex = 0;

// 객체 생성
OneWire oneWire(ONE_WIRE_BUS);
DS18B20_Sensor sensorManager(&oneWire);
Communicator comm;
DataProcessor dataProcessor(&sensorManager);

unsigned long previousMillis = 0;
unsigned long menuDisplayMillis = 0;
bool forceMenuPrint = false;

// 함수 프로토타입 선언
void initializeSerial();
void initialize18b20();
void showDeviceTable();
void show18b20Table_loop();
void handleUserCommands();
void showTroubleshootingGuide();
void displayIdChangeMenu();
void processMenuInput();
void processEditIndividualId();
void processEditSelectiveId();
void processAutoAssignIds();
void resetAllSensorIds();
void parseSensorSelection(String input);
void printCurrentSensorReport();

void setup()
{
    initializeSerial();
    initialize18b20();
    showDeviceTable();
    currentAppState = NORMAL_OPERATION;
}

void loop()
{
    switch (currentAppState)
    {
    case NORMAL_OPERATION:
        show18b20Table_loop();
        handleUserCommands();
        break;
    case MENU_ACTIVE:
        displayIdChangeMenu();
        processMenuInput();
        break;
    case EDIT_INDIVIDUAL_ID:
        processEditIndividualId();
        break;
    case EDIT_SELECTIVE_ID:
        processEditSelectiveId();
        break;
    case AUTO_ASSIGN_IDS:
        processAutoAssignIds();
        break;
    case RESET_ALL_IDS:
        resetAllSensorIds();
        break;
    }
}

void initializeSerial()
{
    comm.begin();
    comm.println("Serial communication initialized.");
    log_info("Serial communication started");
}

void initialize18b20()
{
    sensorManager.begin();
    int found = sensorManager.getDeviceCount();
    if (found == 0)
    {
        log_error("No DS18B20 sensors found. Please check connections.");
        while (true)
        {
            delay(1000);
        }
    }
}

void showDeviceTable()
{
    dataProcessor.printSensorTable();
}

void show18b20Table_loop()
{
    if (millis() - previousMillis >= SENSOR_READ_INTERVAL)
    {
        previousMillis = millis();
        printCurrentSensorReport();
    }
}

void printCurrentSensorReport()
{
    comm.println("\n\n==================== NEW REPORT ====================");
    sensorManager.begin();
    dataProcessor.printSensorTable();
    showTroubleshootingGuide();
    if (dataProcessor.hasInvalidSensors())
    {
        comm.println("---------------------------------------------------------------------------------------------");
        comm.println("Currently identified invalid ID sensors are as follows:");
        dataProcessor.printInvalidSensorTable();
        comm.println("---------------------------------------------------------------------------------------------");
    }
}

void handleUserCommands()
{
    if (comm.isSerialAvailable())
    {
        String command = comm.readStringUntil('\n');
        command.trim();
        command.toLowerCase();
        if (command == "menu" || command == "m")
        {
            comm.println("ID 변경 메뉴를 호출합니다...");
            currentAppState = MENU_ACTIVE;
            comm.clearInputBuffer(); // Clear buffer when entering menu state
            forceMenuPrint = true;
            menuDisplayMillis = millis();
        }
        else if (command == "help" || command == "h")
        {
            comm.println("-----------------------------");
            comm.println("사용 가능한 명령어:");
            comm.println("menu 또는 m : ID 변경 메뉴 호출");
            comm.println("help 또는 h : 도움말 표시");
            comm.println("-----------------------------");
        }
        else if (command.length() > 0)
        {
            comm.print("알 수 없는 명령어: ");
            comm.println(command);
        }
    }
}

void showTroubleshootingGuide() { /* ... (내용 동일) ... */ }

void displayIdChangeMenu()
{
    if (forceMenuPrint || (millis() - menuDisplayMillis >= MENU_REPRINT_INTERVAL))
    {
        comm.println("-------------------------------");
        comm.println("Sensor ID Change Menu:");
        comm.println("1. Change Individual Sensor ID");
        comm.println("2. Change Selective Sensor IDs");
        comm.println("3. Auto-assign IDs by Address");
        comm.println("4. Reset All Sensor IDs");
        comm.println("5. Cancel");
        comm.println("Enter the number of the desired menu and press Enter.");
        comm.println("Menu will timeout in 60 seconds if no input is received.");
        comm.println("-------------------------------");
        menuDisplayMillis = millis();
        forceMenuPrint = false;
    }

    // Check for menu timeout
    if (millis() - menuDisplayMillis >= UI_TIMEOUT)
    {
        comm.println("⏰ Menu timeout. Returning to normal operation.");
        currentAppState = NORMAL_OPERATION;
        comm.clearInputBuffer();
        printCurrentSensorReport();
    }
}

void processMenuInput()
{
    // Check for timeout first in processMenuInput as well
    if (millis() - menuDisplayMillis >= UI_TIMEOUT)
    {
        comm.println("⏰ Menu timeout. Returning to normal operation.");
        currentAppState = NORMAL_OPERATION;
        comm.clearInputBuffer();
        printCurrentSensorReport();
        return;
    }

    String input = comm.readNonBlockingLine();
    if (input.length() > 0)
    {
        if (input.length() == 1 && input[0] >= '1' && input[0] <= '5')
        {
            int num = input.toInt();
            switch (num)
            {
            case 1:
                currentAppState = EDIT_INDIVIDUAL_ID;
                break;
            case 2:
                currentAppState = EDIT_SELECTIVE_ID;
                break;
            case 3:
                currentAppState = AUTO_ASSIGN_IDS;
                break;
            case 4:
                currentAppState = RESET_ALL_IDS;
                break;
            case 5:
                comm.println("Cancelled. Returning to normal operation.");
                currentAppState = NORMAL_OPERATION;
                comm.clearInputBuffer();    // Clear buffer on cancel
                printCurrentSensorReport(); // Immediately print report
                break;
            }
            if (currentAppState != NORMAL_OPERATION)
            {
                currentEditIdSubState = EDIT_ID_NONE;
            }
        }
        else
        {
            comm.println("🧼 Invalid input. Enter a number between 1 and 5.");
        }
    }
}

void processEditIndividualId()
{
    // Check for timeout first in processEditIndividualId as well
    if (millis() - menuDisplayMillis >= UI_TIMEOUT)
    {
        comm.println("⏰ Menu timeout. Returning to normal operation.");
        currentAppState = NORMAL_OPERATION;
        comm.clearInputBuffer();
        printCurrentSensorReport();
        return;
    }

    if (currentEditIdSubState == EDIT_ID_NONE)
    {
        comm.println();
        comm.println("--- Individual Sensor ID Change ---");

        // Check if there are any valid sensors with assigned IDs
        int found = sensorManager.getDeviceCount();
        bool hasValidSensors = false;
        for (int i = 0; i < found; i++)
        {
            DeviceAddress addr;
            sensorManager.getAddress(addr, i);
            int id = sensorManager.getUserData(addr);
            if (id >= 1 && id <= SENSOR_COUNT)
            {
                hasValidSensors = true;
                break;
            }
        }

        if (!hasValidSensors)
        {
            comm.println("No sensors with valid IDs found. Please assign IDs first using menu option 3 or 4.");
            currentAppState = NORMAL_OPERATION;
            comm.clearInputBuffer();
            printCurrentSensorReport();
            return;
        }

        dataProcessor.printSensorTable(); // Show all sensors
        comm.println("Enter the number of the sensor you want to change (1-" + String(SENSOR_COUNT) + "). 0 to cancel.");
        currentEditIdSubState = EDIT_ID_SELECT_SENSOR;
    }

    if (currentEditIdSubState == EDIT_ID_SELECT_SENSOR)
    {
        String input = comm.readNonBlockingLine();
        if (input.length() > 0)
        {
            int sensorNum = input.toInt();
            if (sensorNum == 0)
            {
                comm.println("Cancelling individual ID change.");
                currentAppState = NORMAL_OPERATION;
                comm.clearInputBuffer();    // Clear buffer on cancel
                previousMillis = 0;         // Reset timer for immediate table print
                printCurrentSensorReport(); // Immediately print report
                return;
            }
            if (sensorNum >= 1 && sensorNum <= SENSOR_COUNT)
            {
                // Find the actual DeviceAddress for the selected sensor slot (based on its assigned ID)
                DeviceAddress targetAddr;
                bool foundSensorInSlot = false;
                int foundSensorsCount = sensorManager.getDeviceCount();
                for (int i = 0; i < foundSensorsCount; i++)
                {
                    DeviceAddress currentAddr;
                    sensorManager.getAddress(currentAddr, i);
                    if (sensorManager.getUserData(currentAddr) == sensorNum)
                    { // Match by assigned ID
                        memcpy(targetAddr, currentAddr, 8);
                        foundSensorInSlot = true;
                        break;
                    }
                }

                if (foundSensorInSlot)
                {
                    memcpy(sensorsToChangeArray[0].addr, targetAddr, 8);
                    sensorsToChangeArray[0].currentId = sensorManager.getUserData(targetAddr);
                    sensorsToChangeArray[0].index = sensorNum; // This is the slot number, not physical index

                    comm.println("You selected sensor No. " + String(sensorNum) + " (Current ID: " + String(sensorsToChangeArray[0].currentId) + ").");
                    dataProcessor.printAvailableIds();
                    comm.print("Enter new ID (1-" + String(SENSOR_COUNT) + "): ");
                    currentEditIdSubState = EDIT_ID_ENTER_NEW_ID;
                }
                else
                {
                    // Handle case where no active sensor is found in the selected slot (e.g., empty slot or invalid ID)
                    comm.println("No active sensor found in slot " + String(sensorNum) + ". Please select an active sensor.");
                }
            }
            else // Invalid sensor number (out of 1-8 range)
            {
                comm.println("Invalid sensor number. Please try again.");
            }
        }
    }

    if (currentEditIdSubState == EDIT_ID_ENTER_NEW_ID)
    {
        String input = comm.readNonBlockingLine();
        if (input.length() > 0)
        {
            log_debug((String("Input received for new ID: ") + input).c_str());
            int newId = input.toInt();
            log_debug((String("Parsed new ID: ") + String(newId)).c_str());
            if (newId < 1 || newId > SENSOR_COUNT)
            {
                log_debug("New ID failed range check.");
                comm.println("ID must be between 1 and " + String(SENSOR_COUNT) + ". Please try again.");
                editIdTimeoutMillis = millis(); // Reset timeout on invalid input
                return;
            }
            log_debug((String("Checking if ID ") + String(newId) + " is already used.").c_str());
            if (sensorManager.isIdUsed(newId))
            {
                log_debug((String("ID ") + String(newId) + " is already in use.").c_str());
                comm.println("ID " + String(newId) + " is already in use. Please choose another ID.");
                editIdTimeoutMillis = millis(); // Reset timeout on invalid input
                return;
            }

            DeviceAddress addr;
            memcpy(addr, sensorsToChangeArray[0].addr, 8);
            log_debug((String("Attempting to set user data for address: ") + String(addr[0], HEX) + ":... to ID " + String(newId)).c_str());
            sensorManager.setUserData(addr, newId);
            delay(20); // Give sensor time to write to EEPROM
            log_debug("setUserData called. Verifying change...");

            comm.println("Sensor No. " + String(sensorsToChangeArray[0].index) + " ID changed from " + String(sensorsToChangeArray[0].currentId) + " to " + String(newId) + ".");
            dataProcessor.printSensorTable();
            currentAppState = NORMAL_OPERATION;
            comm.clearInputBuffer();    // Clear buffer after successful change
            previousMillis = 0;         // Reset timer for immediate table print
            printCurrentSensorReport(); // Immediately print report
        }
    }
}

void processEditSelectiveId()
{
    // Check for timeout first in processEditSelectiveId as well
    if (millis() - menuDisplayMillis >= UI_TIMEOUT)
    {
        comm.println("⏰ Menu timeout. Returning to normal operation.");
        currentAppState = NORMAL_OPERATION;
        comm.clearInputBuffer();
        printCurrentSensorReport();
        return;
    }

    if (currentEditIdSubState == EDIT_ID_NONE)
    {
        comm.println();
        comm.println("--- Selective Sensor ID Change ---");

        // Check if there are any valid sensors with assigned IDs
        int found = sensorManager.getDeviceCount();
        bool hasValidSensors = false;
        for (int i = 0; i < found; i++)
        {
            DeviceAddress addr;
            sensorManager.getAddress(addr, i);
            int id = sensorManager.getUserData(addr);
            if (id >= 1 && id <= SENSOR_COUNT)
            {
                hasValidSensors = true;
                break;
            }
        }

        if (!hasValidSensors)
        {
            comm.println("No sensors with valid IDs found. Please assign IDs first using menu option 3 or 4.");
            currentAppState = NORMAL_OPERATION;
            comm.clearInputBuffer();
            printCurrentSensorReport();
            return;
        }

        dataProcessor.printSensorTable(); // Show all sensors
        comm.println("Enter the numbers of the sensors you want to change (e.g., 1 3 5 or 2,4,6). 0 to cancel.");
        currentEditIdSubState = EDIT_SELECTIVE_GET_SENSORS;
        editIdTimeoutMillis = millis();
    }

    if (currentEditIdSubState == EDIT_SELECTIVE_GET_SENSORS)
    {
        if (millis() - editIdTimeoutMillis > UI_TIMEOUT)
        {
            comm.println("⏰ Input timeout. Cancelling.");
            currentAppState = NORMAL_OPERATION;
            comm.clearInputBuffer(); // Clear buffer on timeout
            return;
        }
        String input = comm.readNonBlockingLine();
        if (input.length() > 0)
        {
            if (input == "0")
            {
                comm.println("Cancelling.");
                currentAppState = NORMAL_OPERATION;
                comm.clearInputBuffer();    // Clear buffer on cancel
                previousMillis = 0;         // Reset timer for immediate table print
                printCurrentSensorReport(); // Immediately print report
                return;
            }
            parseSensorSelection(input);
            if (selectedSensorIndicesCount == 0)
            {
                comm.println("No valid sensors selected. Please try again.");
                currentEditIdSubState = EDIT_ID_NONE;
                return;
            }
            comm.print("Selected sensors: ");
            for (int i = 0; i < selectedSensorIndicesCount; i++)
            {
                comm.print(String(sensorsToChangeArray[selectedSensorIndices[i]].index));
                if (i < selectedSensorIndicesCount - 1)
                    comm.print(", ");
            }
            comm.println();
            comm.print("Proceed with these selections? (y/n): ");
            currentEditIdSubState = EDIT_SELECTIVE_CONFIRM;
            editIdTimeoutMillis = millis();
        }
    }

    if (currentEditIdSubState == EDIT_SELECTIVE_CONFIRM)
    {
        if (millis() - editIdTimeoutMillis > UI_TIMEOUT)
        {
            comm.println("⏰ Input timeout. Cancelling.");
            currentAppState = NORMAL_OPERATION;
            comm.clearInputBuffer(); // Clear buffer on timeout
            return;
        }
        String input = comm.readNonBlockingLine();
        if (input.length() > 0)
        {
            if (input.equalsIgnoreCase("y"))
            {
                currentSelectiveIndex = 0;
                currentEditIdSubState = EDIT_SELECTIVE_ENTER_NEW_IDS;
                dataProcessor.printAvailableIds();
                int sensorTableIndex = sensorsToChangeArray[selectedSensorIndices[currentSelectiveIndex]].index;
                comm.print("Enter new ID for sensor No. " + String(sensorTableIndex) + ": ");
                editIdTimeoutMillis = millis();
            }
            else
            {
                comm.println("Cancelled. Restarting selection.");
                currentEditIdSubState = EDIT_ID_NONE;
                comm.clearInputBuffer();    // Clear buffer on cancel
                previousMillis = 0;         // Reset timer for immediate table print
                printCurrentSensorReport(); // Immediately print report
            }
        }
    }

    if (currentEditIdSubState == EDIT_SELECTIVE_ENTER_NEW_IDS)
    {
        if (millis() - editIdTimeoutMillis > UI_TIMEOUT)
        {
            comm.println("⏰ Input timeout. Cancelling.");
            currentAppState = NORMAL_OPERATION;
            comm.clearInputBuffer(); // Clear buffer on timeout
            return;
        }
        String input = comm.readNonBlockingLine();
        if (input.length() > 0)
        {
            int newId = input.toInt();
            if (newId < 1 || newId > SENSOR_COUNT || sensorManager.isIdUsed(newId))
            {
                comm.println("Invalid or already used ID. Please try again.");
                editIdTimeoutMillis = millis();
                return;
            }
            int arrayIndex = selectedSensorIndices[currentSelectiveIndex];
            DeviceAddress addr;
            memcpy(addr, sensorsToChangeArray[arrayIndex].addr, 8);
            sensorManager.setUserData(addr, newId);
            delay(20); // Give sensor time to write to EEPROM

            int sensorTableIndex = sensorsToChangeArray[arrayIndex].index;
            comm.println("Sensor No. " + String(sensorTableIndex) + " ID changed to " + String(newId));

            currentSelectiveIndex++;
            if (currentSelectiveIndex >= selectedSensorIndicesCount)
            {
                comm.println("All selected sensors have been updated.");
                dataProcessor.printSensorTable();
                currentAppState = NORMAL_OPERATION;
                previousMillis = 0; // Reset timer for immediate table print
            }
            else
            {
                dataProcessor.printAvailableIds();
                int nextSensorTableIndex = sensorsToChangeArray[selectedSensorIndices[currentSelectiveIndex]].index;
                comm.print("Enter new ID for sensor No. " + String(nextSensorTableIndex) + ": ");
                editIdTimeoutMillis = millis();
            }
        }
    }
}

void processAutoAssignIds()
{
    comm.println("--- Auto-assigning IDs by Address ---");
    int found = sensorManager.getDeviceCount();
    if (found == 0)
    {
        comm.println("No sensors found.");
        currentAppState = NORMAL_OPERATION;
        return;
    }

    // 1. Fetch all addresses
    DeviceAddress addrs[found];
    for (int i = 0; i < found; i++)
    {
        sensorManager.getAddress(addrs[i], i);
    }

    // 2. Sort addresses using Bubble Sort
    for (int i = 0; i < found - 1; i++)
    {
        for (int j = 0; j < found - i - 1; j++)
        {
            if (memcmp(addrs[j], addrs[j + 1], 8) > 0)
            {
                DeviceAddress temp;
                memcpy(temp, addrs[j], 8);
                memcpy(addrs[j], addrs[j + 1], 8);
                memcpy(addrs[j + 1], temp, 8);
            }
        }
    }

    // 3. Assign new IDs (1 to N)
    for (int i = 0; i < found; i++)
    {
        sensorManager.setUserData(addrs[i], i + 1);
        delay(20);
    }

    comm.println("Auto-assignment complete.");
    dataProcessor.printSensorTable();
    currentAppState = NORMAL_OPERATION;
    previousMillis = 0; // Reset timer for immediate table print
}

void resetAllSensorIds()
{
    comm.println("--- Resetting all sensor IDs ---");
    int found = sensorManager.getDeviceCount();
    for (int i = 0; i < found; i++)
    {
        DeviceAddress addr;
        if (sensorManager.getAddress(addr, i))
        {
            sensorManager.setUserData(addr, 0);
            delay(10);
        }
    }
    comm.println("All sensor IDs have been reset to 0.");
    comm.println("You can now use 'Auto-assign' or other menu options to set new IDs.");

    // Print table of sensors with now-invalid IDs
    comm.println("--- Connected Sensors (Address List) ---");
    dataProcessor.printInvalidSensorTable();

    currentAppState = NORMAL_OPERATION;
    previousMillis = 0; // Reset timer for immediate table print
}

void parseSensorSelection(String input)
{
    // First, populate the list of all available sensors that have valid IDs
    sensorsToChangeCount = 0;
    int totalFoundSensors = sensorManager.getDeviceCount();

    // Collect only sensors with valid IDs (1-8)
    for (int i = 0; i < totalFoundSensors; i++)
    {
        DeviceAddress addr;
        sensorManager.getAddress(addr, i);
        int currentId = sensorManager.getUserData(addr);
        if (currentId >= 1 && currentId <= SENSOR_COUNT)
        {
            sensorsToChangeArray[sensorsToChangeCount].index = currentId; // Use the actual ID as index
            sensorsToChangeArray[sensorsToChangeCount].currentId = currentId;
            memcpy(sensorsToChangeArray[sensorsToChangeCount].addr, addr, 8);
            sensorsToChangeCount++;
        }
    }

    // Now, parse the user's selection with improved parsing logic
    selectedSensorIndicesCount = 0;
    bool isSelected[SENSOR_COUNT + 1] = {false};

    // Replace commas with spaces to simplify parsing
    input.replace(',', ' ');
    input.trim();

    // Enhanced parsing to handle multi-digit numbers and consecutive digits
    // First try to parse as space/comma separated numbers
    input.replace(',', ' ');
    input.trim();

    // Check if input contains spaces or is purely consecutive digits
    bool hasSpaces = input.indexOf(' ') >= 0;

    if (hasSpaces)
    {
        // Parse space-separated numbers
        String currentNumber = "";
        for (int i = 0; i <= input.length(); i++)
        {
            char c = (i < input.length()) ? input.charAt(i) : ' ';
            if (isDigit(c))
            {
                currentNumber += c;
            }
            else if (c == ' ')
            {
                if (currentNumber.length() > 0)
                {
                    int id = currentNumber.toInt();
                    if (id >= 1 && id <= SENSOR_COUNT)
                    {
                        for (int j = 0; j < sensorsToChangeCount; j++)
                        {
                            if (sensorsToChangeArray[j].currentId == id)
                            {
                                isSelected[id] = true;
                                break;
                            }
                        }
                    }
                    currentNumber = "";
                }
            }
        }
    }
    else
    {
        // Parse consecutive digits as individual numbers (e.g., "78" -> 7, 8)
        for (int i = 0; i < input.length(); i++)
        {
            char c = input.charAt(i);
            if (isDigit(c))
            {
                int id = c - '0'; // Convert single digit to number
                if (id >= 1 && id <= SENSOR_COUNT)
                {
                    for (int j = 0; j < sensorsToChangeCount; j++)
                    {
                        if (sensorsToChangeArray[j].currentId == id)
                        {
                            isSelected[id] = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Populate selectedSensorIndices array
    for (int i = 1; i <= SENSOR_COUNT; i++)
    {
        if (isSelected[i])
        {
            // Find the array index for this ID
            for (int j = 0; j < sensorsToChangeCount; j++)
            {
                if (sensorsToChangeArray[j].currentId == i)
                {
                    selectedSensorIndices[selectedSensorIndicesCount++] = j;
                    break;
                }
            }
        }
    }
}