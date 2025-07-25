# DS18B20 Temperature Sensor Management System (v6.08)

## 1. 개요
이 프로젝트는 최대 8개의 DS18B20 온도 센서를 관리하는 아두이노 시스템입니다. 클래스 기반으로 코드를 모듈화하여 가독성과 유지보수성을 높였으며, 시리얼 통신을 통해 사용자와 상호작용하여 센서 ID를 관리하고 상태를 모니터링하는 기능을 제공합니다. v6.08에서는 메뉴 시스템의 안정성과 사용자 경험이 크게 향상되었습니다.

## 2. 주요 기능
- **자동 센서 모니터링:** 15초마다 모든 센서의 상태(온도, ID, 연결 상태)를 읽어와 테이블 형태로 출력합니다.
- **상세한 상태 보고:** 온도 임계값 초과/미달, 센서 미연결, 비정상 ID 등 다양한 상태를 감지하고 문제 해결 가이드를 제공합니다.
- **동적 ID 관리:** 시리얼 명령어를 통해 다음과 같은 ID 변경 기능을 지원합니다.
  - **개별 ID 변경:** 연결된 모든 센서 중 하나를 선택하여 ID를 변경합니다.
  - **선택적 ID 변경:** 변경을 원하는 여러 센서를 한 번에 선택하여 ID를 순차적으로 변경합니다. (향상된 입력 파싱 및 확인 절차 포함)
  - **자동 ID 할당:** 연결된 모든 센서에 1부터 순차적으로 ID를 자동 할당합니다.
  - **전체 ID 초기화:** 모든 센서의 ID를 0으로 리셋합니다.
- **즉각적인 메뉴 응답:** 사용자가 메뉴 호출 시 지연 없이 즉시 메뉴가 표시되도록 응답성을 개선했습니다.
- **안정적인 메뉴 시스템:** 60초 타임아웃 기능과 향상된 입력 처리로 메뉴 사용 중 멈춤 현상을 방지합니다.
- **강화된 입력 파싱:** "78"→"7,8", "123"→"1,2,3" 등 다양한 형태의 센서 번호 입력을 정확하게 해석합니다.

## 3. 사용법

### 3.1. 하드웨어 연결
```
DS18B20 ------------- Arduino
VCC    ------------- 5V
GND    ------------- GND
DQ     ------------- D2
```

### 3.2. 시리얼 모니터 명령어
- `menu` 또는 `m`: ID 변경 메뉴를 호출합니다.
- `help` 또는 `h`: 사용 가능한 명령어 목록을 봅니다.

### 3.3. ID 변경 절차
1. `menu` 또는 `m`을 입력하여 ID 변경 메뉴에 진입합니다.
2. 원하는 기능의 번호를 입력합니다:
   - `1`: 개별 센서 ID 변경
   - `2`: 선택적 센서 ID 변경 (여러 센서 동시 선택)
   - `3`: 자동 ID 할당 (1부터 순차 할당)
   - `4`: 전체 ID 초기화 (모든 센서 ID를 0으로 리셋)
   - `5`: 취소 및 일반 모드 복귀
3. 화면의 안내에 따라 변경할 센서 번호와 새로운 ID를 입력합니다.
4. 메뉴는 60초 후 자동으로 타임아웃되어 일반 모드로 복귀합니다.

## 4. 개발 및 변경 로그 (v6.08)

### 4.1. v6.08 최신 개선사항 (2025-07-24)
v6.08에서는 checkList6_07.md에서 확인된 주요 문제점들을 해결하여 메뉴 시스템의 안정성과 사용자 경험을 크게 향상시켰습니다.

#### **핵심 수정사항:**
1. **메뉴 타임아웃 기능 안정화** ✅
   - `processMenuInput()` 함수에 타임아웃 체크 로직 추가로 이중 안전장치 구현
   - 메뉴에서 60초 타임아웃이 안정적으로 동작하도록 개선

2. **향상된 입력 파싱 시스템** ✅
   - "78"→"7,8", "123"→"1,2,3" 등 붙여쓰기 숫자의 자동 분리 처리
   - `parseSensorSelection()` 함수의 완전한 재작성으로 모든 입력 형태 지원
   - 잘못된 입력에 대한 명확한 오류 메시지 제공

3. **사용자 인터페이스 개선** ✅
   - 메뉴 취소 시 즉시 센서 테이블 출력으로 응답성 향상
   - 입력 버퍼 관리 최적화로 메뉴 멈춤 현상 완전 해결
   - 타임아웃 발생 시 명확한 안내 메시지 ("⏰ Menu timeout. Returning to normal operation.")

### 4.2. v6 시리즈 주요 발전 과정

#### **v6.01-v6.05: 기초 기능 구현**
- **선택적 ID 변경 기능 구현:** 다중 센서 선택 및 순차 변경 기능 추가
- **메뉴 응답 속도 개선:** `forceMenuPrint` 플래그로 즉시 메뉴 출력 구현
- **입력 파싱 개선:** 다양한 구분자(공백, 쉼표) 지원
- **메뉴 안정성 강화:** `clearInputBuffer()` 함수로 입력 먹통 현상 해결

#### **v6.06-v6.07: 안정화 및 최적화**
- **센서 테이블 즉시 업데이트:** 모든 상태 전환 시 지연 없는 테이블 출력
- **사용자 경험 향상:** 취소 시 즉시 피드백, 명확한 상태 안내
- **코드 최적화:** 중복 코드 제거 및 함수 구조 개선

### 4.3. v6 목표 달성도
✅ **완료된 목표:**
1. ID 변경 대상을 정상 ID 센서까지 확장하고, 여러 센서를 선택하여 변경하는 기능 추가
2. `menu` 명령어 입력 시 발생하는 15초의 응답 지연 문제 해결
3. 메뉴 시스템의 안정성 및 사용자 경험 대폭 개선
4. 복잡한 센서 번호 입력 형태에 대한 완벽한 파싱 지원

## 5. 파일 구조
- **설정:** `config.h`
- **메인 스케치:** `01_ds18b20_flow_06.ino` (v6.08)
- **클래스 모듈:**
  - `DS18B20_Sensor.h/.cpp` (하드웨어 제어)
  - `DataProcessor.h/.cpp` (데이터 처리 및 출력)
  - `Communicator.h/.cpp` (시리얼 통신)
  - `Utils.h/.cpp` (공용 함수)
- **개발 문서:**
  - `README.md` (이 파일)
  - `changeLog/` 폴더: 버전별 상세 변경 로그 (changelog5to6.md ~ changelog6to6_08.md)
  - `checkList/` 폴더: 버전별 기능 검증 체크리스트 (checkList6_01.md ~ checkList6_08.md)
  - `adviceFromAi/`, `continueChat/` 폴더: 개발 과정 기록

## 6. 기술 사양
- **지원 센서:** DS18B20 온도 센서 (최대 8개)
- **통신 방식:** OneWire 프로토콜 (D2 핀)
- **업데이트 주기:** 15초마다 자동 센서 상태 확인
- **메뉴 타임아웃:** 60초 (UI_TIMEOUT)
- **시리얼 통신:** 9600 baud
- **온도 범위:** DS18B20 센서 사양에 따름 (-55°C ~ +125°C)

## 7. 의존성
- OneWire 라이브러리
- DallasTemperature 라이브러리

## 8. 버전 히스토리
- **v6.08** (2025-07-24): 메뉴 타임아웃 안정화, 향상된 입력 파싱, UI 개선
- **v6.07** (2025-07-24): 선택적 ID 변경 기능 완성, 메뉴 시스템 안정화
- **v6.06** (2025-07-24): 사용자 경험 최적화, 즉시 테이블 업데이트
- **v6.05** (2025-07-24): 메뉴 입력 먹통 현상 해결, 파싱 로직 완벽 개선
- **v6.04** (2025-07-24): 입력 파싱 개선, 메뉴 안정성 강화
- **v6.01-v6.03** (2025-07-24): 선택적 ID 변경 기능 구현, 메뉴 응답 속도 개선
- **v6.00** (2025-07-23): 클래스 기반 코드 모듈화, 기본 기능 구현

---
*최종 업데이트: 2025-07-25 (v6.08 기준)*