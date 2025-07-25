# v6.02 기능 검증 테스트 체크리스트

이 문서는 `01_ds18b20_flow_06` 펌웨어의 수정된 기능들이 의도대로 작동하는지 확인하기 위한 테스트 절차를 정의합니다.

## 1. 사전 준비
1.  수정된 펌웨어를 Arduino 보드에 업로드합니다.
2.  Arduino IDE의 시리얼 모니터를 열고 보드레이트를 `9600`으로 설정합니다.

## 2. 테스트 시나리오

### 시나리오 1: 초기 상태 및 메뉴 응답성 검증
- [v] **(1-1) 초기화:** 시리얼 모니터에 초기화 메시지와 함께 전체 센서 테이블이 정상적으로 출력되는지 확인합니다.
- [v] **(1-2) 주기적 업데이트:** 약 15초마다 센서 테이블이 자동으로 업데이트되는지 확인합니다.
- [v] **(1-3) 메뉴 즉시 응답 테스트:**
  - 시리얼 모니터에 `menu` 또는 `m`을 입력하고 엔터를 누릅니다.
  - **기대 결과:** "ID 변경 메뉴를 호출합니다..." 메시지와 함께 ID 변경 메뉴가 **지연 없이 즉시** 출력되어야 합니다.
- [v] **(1-4) 메뉴 주기적 재출력:** 메뉴 상태에서 아무것도 입력하지 않았을 때, 약 15초 간격으로 메뉴가 다시 출력되는지 확인합니다.

### 시나리오 2: 개별 ID 변경 기능 검증 (범위 확장)
- [v] **(2-1) 메뉴 진입:** 메뉴에서 `1`을 입력하여 '개별 센서 ID 변경'으로 진입합니다.
- [v] **(2-2) 센서 목록 확인:**
  - **기대 결과:** "List of all connected sensors:" 메시지와 함께, ID의 정상/비정상 여부와 관계없이 **연결된 모든 센서**의 목록이 출력되어야 합니다.
- [v] **(2-3) 정상 ID 센서 변경:** 목록에서 ID가 정상인 센서의 번호를 입력하고, 안내에 따라 사용하지 않는 새 ID(예: 7)를 입력합니다.
  - **기대 결과:** ID가 성공적으로 변경되고, 업데이트된 센서 테이블이 출력되어야 합니다.
- [v] **(2-4) 비정상 ID 센서 변경:** (만약 있다면) 목록에서 ID가 비정상인 센서의 번호를 입력하고, 안내에 따라 유효한 새 ID를 입력합니다.
  - **기대 결과:** ID가 성공적으로 변경되어야 합니다.
- [v] **(2-5) 취소:** 센서 선택 화면에서 `0`을 입력했을 때, "Cancelling..." 메시지와 함께 주 메뉴로 돌아가는지 확인합니다.

### 시나리오 3: 선택적 ID 변경 기능 검증 (신규 기능)
- [v] **(3-1) 메뉴 진입:** 메뉴에서 `2`를 입력하여 '선택적 ID 변경'으로 진입합니다.
- **(3-2) 다양한 형식의 입력 테스트:**
  - 센서 선택 프롬프트에서 `1 3 5` (공백 구분)를 입력합니다.
  [v] **기대 결과:** "Selected sensors: 1, 3, 5" 메시지가 출력되어야 합니다.
  - 다시 시도하여 `2,4,6` (쉼표 구분)을 입력합니다.
  [v] **기대 결과:** "Selected sensors: 2, 4, 6" 메시지가 출력되어야 합니다.
  다시 시도하여 `1231` (붙여쓰기 및 중복)을 입력합니다.
  [v] **기대 결과:** "Selected sensors: 1, 2, 3" (고유값만 인식) 메시지가 출력되어야 합니다.
  [v] 다시 시도하여 `a1, b2!, 8` (특수문자/알파벳 포함)을 입력합니다.
  - **기대 결과:** "Selected sensors: 1, 2, 8" (숫자만 파싱) 메시지가 출력되어야 합니다.   
- [v] **(3-3) 사용자 확인 절차 테스트:**
  - 센서 선택 후 나타나는 "Proceed with these selections? (y/n):" 프롬프트에서 `n`을 입력합니다.
  [v] **기대 결과:** "Cancelled. Restarting selection." 메시지와 함께 센서 선택 단계로 돌아가야 합니다.
  - 다시 센서를 선택하고, 프롬프트에서 `y`를 입력합니다.
  - **기대 결과:** ID 변경 단계로 정상적으로 진행되어야 합니다.
- [v] **(3-4) 순차적 ID 변경 테스트:**
  - `y`를 입력한 후, 첫 번째 선택된 센서(예: 1번)의 새 ID를 입력하라는 프롬프트가 표시되는지 확인합니다.
  [v] 유효한 새 ID를 입력하면, 다음 선택된 센서(예: 3번)의 새 ID를 입력하라는 프롬프트가 표시되는지 확인합니다.
  [x] 모든 선택된 센서의 ID 변경이 완료되면, "All selected sensors have been updated." 메시지와 함께 업데이트된 전체 센서 테이블이 출력되는지 확인합니다.
  - 첫번째 변경은 되지만 같은 센서에 2번째 변경을 시도 하면 변경이 안된다.
- [v] **(3-5) 타임아웃 테스트:** 각 입력 단계(센서 선택, y/n 확인, 새 ID 입력)에서 60초간 아무것도 입력하지 않았을 때, "Input timeout. Cancelling." 메시지와 함께 주 메뉴로 돌아가는지 확인합니다.

### 시나리오 4: 기타 기능 회귀 테스트
- [v] **(4-1) 전체 ID 초기화:** 메뉴에서 `3`을 입력했을 때, 모든 센서의 ID가 0으로 초기화되고 업데이트된 테이블이 출력되는지 확인합니다.
  초기화 이후 전체 테이블에는 안 보이지만 연결된 센서들의 어드레스를 보이게 하는 테이블이 추가 되어야 한다.
  그리고 초기화 된 센서들에 대해서 새로운 id를 부여 하는 과정에서는 기준이 되는 id가 없기에 이때는 어드레스 기준으로 선택을 할 수 있는 과정이 필요하다.
- [v] **(4-2) 메뉴 취소:** 메뉴에서 `4`를 입력했을 때, "Exit ID Change Menu." 메시지와 함께 일반 모니터링 상태로 돌아가는지 확인합니다.


### 추가 했으면 하는 기능

개별 id 변경하는 것처럼
선택적 id 변경 때에도 초기화 되었던 센서 들이나 비정상 id 를 가진 센서들의 목록을 보이게 해주어야 한다.

현재 코드를 실행해보니 개별 id 변경 기능에는 있는게 선택적 id 변경 메뉴 및 프로세스에는 없다.

그리고 최상위 메뉴 4가지 에서 5가지로 늘리면 하는게 있는데
1, 2 메뉴는 놔두고 
3번에 어드레스 오름차순에 의해서 id 값을 자동 부여하는 기능을 넣고 
4번에 기존 의 전체 id 초기화 기능을 넣고
5번은 취소 로 했으면 한다.

