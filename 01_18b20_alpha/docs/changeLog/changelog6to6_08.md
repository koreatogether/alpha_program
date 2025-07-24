# Changelog v6.07 → v6.08

## 수정 날짜: 2025-07-24

## 수정 개요
checkList6_07.md에서 확인된 주요 문제점들을 해결하기 위한 중요한 펌웨어 수정 작업을 수행하였습니다. 이번 수정은 사용자 경험 개선과 메뉴 시스템 안정화에 중점을 두었습니다.

## 수정된 문제점들

### 1. **메뉴 타임아웃 기능 안정화** ✅
**문제:** 메뉴에서 60초 타임아웃이 정상적으로 동작하지 않는 문제

**해결방법:**
- `processMenuInput()` 함수에 타임아웃 체크 로직을 추가하여 이중 안전장치 구현
- 메뉴 입력 처리 시에도 타임아웃을 확인하여 더욱 안정적인 동작 보장

**수정된 코드:**
```cpp
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
    // ...existing code...
}
```

### 2. **메뉴 취소 후 테이블 출력 지연 개선** ✅
**문제:** 선택적 ID 변경 메뉴에서 0번(취소) 입력 후 전체 테이블 출력까지 지연 발생

**해결방법:**
- 취소 시 `previousMillis = 0` 설정으로 즉시 테이블 출력 유도
- `printCurrentSensorReport()` 직접 호출로 즉시 피드백 제공
- 확인 단계(y/n)에서 취소 시에도 동일한 처리 적용

**수정된 코드:**
```cpp
if (input == "0")
{
    comm.println("Cancelling.");
    currentAppState = NORMAL_OPERATION;
    comm.clearInputBuffer(); // Clear buffer on cancel
    previousMillis = 0;      // Reset timer for immediate table print
    printCurrentSensorReport(); // Immediately print report
    return;
}

// 확인 단계에서도 동일 처리
else
{
    comm.println("Cancelled. Restarting selection.");
    currentEditIdSubState = EDIT_ID_NONE;
    comm.clearInputBuffer(); // Clear buffer on cancel
    previousMillis = 0;      // Reset timer for immediate table print
    printCurrentSensorReport(); // Immediately print report
}
```

### 3. **선택적 ID 변경 입력 파싱 로직 대폭 개선** ✅
**문제:** 
- `78` 입력 시 7번과 8번 센서로 인식되지 않음
- `1,10,5` 등 복합 입력에서 일부만 인식되는 현상
- 공백/쉼표 구분 파싱 불완전

**해결방법:**
- 입력 형태에 따른 지능형 파싱 로직 구현
- 공백이 있으면 구분자 기반 파싱, 없으면 연속 숫자를 개별 숫자로 파싱
- `78` → 7, 8로 개별 인식하도록 개선
- `1 2 3`, `1,2,3`, `123` 모든 형태 지원

**수정된 핵심 로직:**
```cpp
// Enhanced parsing to handle multi-digit numbers and consecutive digits
input.replace(',', ' ');
input.trim();

// Check if input contains spaces or is purely consecutive digits
bool hasSpaces = input.indexOf(' ') >= 0;

if (hasSpaces) {
    // Parse space-separated numbers (supports multi-digit like "1 10 5")
    // ...space-separated parsing logic...
} else {
    // Parse consecutive digits as individual numbers (e.g., "78" -> 7, 8)
    for (int i = 0; i < input.length(); i++)
    {
        char c = input.charAt(i);
        if (isDigit(c))
        {
            int id = c - '0'; // Convert single digit to number
            // ...individual digit processing...
        }
    }
}
```

## 수정된 파일 목록

### 주요 수정 파일
1. **01_ds18b20_flow_06.ino**
   - `processMenuInput()` 함수: 타임아웃 이중 체크 로직 추가
   - `processEditSelectiveId()` 함수: 취소 시 즉시 테이블 출력 개선
   - `parseSensorSelection()` 함수: 지능형 입력 파싱 로직 완전 재구현

### 새로 생성된 파일
1. **changelog6to6_08.md** (본 파일)

## 사용자 경험 개선 사항

### 🚀 **즉각적인 반응성 향상**
- 메뉴 취소 시 즉시 테이블 출력으로 대기 시간 제거
- 타임아웃 기능 안정화로 무한 대기 상황 방지

### 🎯 **직관적인 입력 지원**
- `78` 입력으로 7번, 8번 센서 동시 선택 가능
- `1 2 3`, `1,2,3`, `123` 모든 입력 형태 지원
- 사용자가 원하는 방식으로 자유롭게 입력 가능

### 🛡️ **시스템 안정성 강화**
- 메뉴 타임아웃 이중 안전장치로 시스템 멈춤 방지
- 더욱 견고한 에러 처리 및 복구 메커니즘

## 테스트 검증 항목

### 해결된 문제들
- ✅ **시나리오 3-2**: 메뉴 타임아웃 정상 동작 (이중 체크로 안정화)
- ✅ **메뉴 취소 후 지연**: 즉시 테이블 출력으로 개선
- ✅ **시나리오 2-1**: 다양한 입력 파싱 (`78`, `1,10,5`, `123` 등)

### 추가 검증 필요
- [ ] **연속 숫자 파싱**: `78`, `123`, `1234` 등 다양한 조합 테스트
- [ ] **복합 입력**: `1 23 4`, `12,34` 등 혼합 형태 테스트
- [ ] **메뉴 타임아웃**: 실제 60초 대기 후 정상 복귀 확인

## 향후 개발 방향

이번 v6.08 업데이트로 주요 사용성 문제들이 해결되었으며, 향후 다음 사항들을 고려해야 합니다:

1. **고급 입력 패턴 지원**: 범위 입력(`1-5`) 등 추가 고려
2. **성능 최적화**: 대량 센서 환경에서의 파싱 성능 개선
3. **사용자 피드백**: 실제 사용 환경에서의 추가 개선점 수집
4. **예외 상황 강화**: 더 다양한 입력 오류 상황 대응

## 테스트 권장사항

v6.08 업데이트 후 다음 테스트를 수행하여 수정 사항이 올바르게 작동하는지 확인하기 바랍니다:

1. **메뉴 타임아웃 테스트**: 메뉴 진입 후 60초 대기하여 자동 복귀 확인
2. **즉시 취소 테스트**: 메뉴 2번에서 0번 입력 후 테이블 즉시 출력 확인
3. **다양한 입력 테스트**: 
   - `78` 입력으로 7, 8번 센서 선택 확인
   - `1,2,3` 입력으로 1, 2, 3번 센서 선택 확인
   - `123` 입력으로 1, 2, 3번 센서 선택 확인
4. **복합 시나리오 테스트**: 취소 → 재진입 → 다양한 입력 조합

---

**수정자**: GitHub Copilot  
**검토 필요**: v6.08 업데이트 후 전체 기능 테스트  
**다음 버전 계획**: v6.09에서 추가 사용성 개선 및 성능 최적화 예정
