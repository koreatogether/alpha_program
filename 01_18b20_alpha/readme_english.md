# DS18B20 Temperature Sensor Management System (v6.08)

## 1. Overview
This project is an Arduino system that manages up to 8 DS18B20 temperature sensors. The code is modularized based on classes to enhance readability and maintainability, and it provides features for managing sensor IDs and monitoring their status through serial communication with the user. In v6.08, the stability and user experience of the menu system have been significantly improved.

## 2. Key Features
- **Automatic Sensor Monitoring:** Reads the status (temperature, ID, connection status) of all sensors every 15 seconds and displays it in a table format.
- **Detailed Status Reporting:** Detects various states such as temperature threshold exceeded/not met, sensor not connected, and abnormal ID, and provides troubleshooting guides.
- **Dynamic ID Management:** Supports the following ID change functions via serial commands:
  - **Individual ID Change:** Select one of the connected sensors to change its ID.
  - **Selective ID Change:** Select multiple sensors you want to change at once and change their IDs sequentially (includes improved input parsing and confirmation procedures).
  - **Automatic ID Assignment:** Automatically assigns IDs sequentially from 1 to all connected sensors.
  - **Reset All IDs:** Resets the IDs of all sensors to 0.
- **Instant Menu Response:** Improved responsiveness so that the menu is displayed immediately without delay when the user calls it.
- **Stable Menu System:** Prevents the menu from freezing during use with a 60-second timeout feature and improved input processing.
- **Enhanced Input Parsing:** Accurately interprets various forms of sensor number input, such as "78"→"7,8" and "123"→"1,2,3".

## 3. Usage

### 3.1. Hardware Connection
```
DS18B20 ------------- Arduino
VCC    ------------- 5V
GND    ------------- GND
DQ     ------------- D2
```

### 3.2. Serial Monitor Commands
- `menu` or `m`: Calls the ID change menu.
- `help` or `h`: Displays a list of available commands.

### 3.3. ID Change Procedure
1. Enter `menu` or `m` to enter the ID change menu.
2. Enter the number of the desired function:
   - `1`: Change individual sensor ID
   - `2`: Change selective sensor IDs (select multiple sensors at once)
   - `3`: Automatic ID assignment (sequential assignment from 1)
   - `4`: Reset all IDs (resets all sensor IDs to 0)
   - `5`: Cancel and return to normal mode
3. Follow the on-screen instructions to enter the sensor number and new ID to be changed.
4. The menu will automatically time out after 60 seconds and return to normal mode.

## 4. Development and Change Log (v6.08)

### 4.1. v6.08 Latest Improvements (2025-07-24)
In v6.08, the key issues identified in checkList6_07.md were resolved to significantly improve the stability and user experience of the menu system.

#### **Core Modifications:**
1. **Menu Timeout Function Stabilization** ✅
   - Implemented a double safety device by adding timeout check logic to the `processMenuInput()` function.
   - Improved the 60-second timeout to operate stably in the menu.

2. **Improved Input Parsing System** ✅
   - Automatic separation of concatenated numbers, such as "78"→"7,8" and "123"→"1,2,3".
   - Complete rewrite of the `parseSensorSelection()` function to support all input formats.
   - Provided clear error messages for incorrect input.

3. **User Interface Improvements** ✅
   - Improved responsiveness by immediately displaying the sensor table upon menu cancellation.
   - Completely resolved the menu freezing issue by optimizing input buffer management.
   - Clear guidance message upon timeout ("⏰ Menu timeout. Returning to normal operation.").

### 4.2. v6 Series Major Development Progress

#### **v6.01-v6.05: Basic Function Implementation**
- **Selective ID Change Function Implementation:** Added multi-sensor selection and sequential change function.
- **Menu Response Speed Improvement:** Implemented immediate menu display with the `forceMenuPrint` flag.
- **Input Parsing Improvement:** Supported various delimiters (spaces, commas).
- **Menu Stability Enhancement:** Resolved the input unresponsiveness issue with the `clearInputBuffer()` function.

#### **v6.06-v6.07: Stabilization and Optimization**
- **Immediate Sensor Table Update:** Table display without delay upon all state transitions.
- **User Experience Improvement:** Immediate feedback upon cancellation, clear status guidance.
- **Code Optimization:** Removed duplicate code and improved function structure.

### 4.3. v6 Goal Achievement
✅ **Completed Goals:**
1. Expanded the ID change target to include normal ID sensors and added the ability to select and change multiple sensors.
2. Resolved the 15-second response delay issue when entering the `menu` command.
3. Significantly improved the stability and user experience of the menu system.
4. Perfect parsing support for complex sensor number input formats.

## 5. File Structure
- **Configuration:** `config.h`
- **Main Sketch:** `01_ds18b20_flow_06.ino` (v6.08)
- **Class Modules:**
  - `DS18B20_Sensor.h/.cpp` (Hardware Control)
  - `DataProcessor.h/.cpp` (Data Processing and Output)
  - `Communicator.h/.cpp` (Serial Communication)
  - `Utils.h/.cpp` (Common Functions)
- **Development Documents:**
  - `README.md` (this file)
  - `changeLog/` folder: Detailed change log for each version (changelog5to6.md ~ changelog6to6_08.md)
  - `checkList/` folder: Function verification checklist for each version (checkList6_01.md ~ checkList6_08.md)
  - `adviceFromAi/`, `continueChat/` folders: Development process records

## 6. Technical Specifications
- **Supported Sensors:** DS18B20 Temperature Sensor (up to 8)
- **Communication Method:** OneWire Protocol (Pin D2)
- **Update Cycle:** Automatic sensor status check every 15 seconds
- **Menu Timeout:** 60 seconds (UI_TIMEOUT)
- **Serial Communication:** 9600 baud
- **Temperature Range:** According to DS18B20 sensor specifications (-55°C ~ +125°C)

## 7. Dependencies
- OneWire library
- DallasTemperature library

## 8. Version History
- **v6.08** (2025-07-24): Menu timeout stabilization, improved input parsing, UI improvements
- **v6.07** (2025-07-24): Selective ID change function completion, menu system stabilization
- **v6.06** (2025-07-24): User experience optimization, immediate table update
- **v6.05** (2025-07-24): Resolved menu input freezing issue, perfected parsing logic
- **v6.04** (2025-07-24): Improved input parsing, enhanced menu stability
- **v6.01-v6.03** (2025-07-24): Implemented selective ID change function, improved menu response speed
- **v6.00** (2025-07-23): Class-based code modularization, basic function implementation

---
*Last updated: 2025-07-25 (based on v6.08)*
