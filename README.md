# ARDUINO-WEB-GUI-TERMINAL_V1

**이 가이드는 하드웨어 제어 경험이 적은 사용자도 쉽게 따라 할 수 있도록 설계되었습니다.**

---

# 🇰🇷 [가이드] Arduino Web Serial GUI 모니터 사용법

이 시스템은 아두이노가 보내는 특정 형식의 메시지를 웹 브라우저가 해석하여 **실시간으로 UI(슬라이더, 스위치, 차트 등)를 생성**하는 동적 대시보드입니다.

## 1. 준비 사항
- **브라우저:** Google Chrome 또는 Microsoft Edge (Web Serial API를 지원하는 최신 버전)
  - *주의: Firefox나 Safari에서는 작동하지 않습니다.*
- **하드웨어:** 아두이노 보드 (Uno, Nano, Mega, ESP32 등 시리얼 통신이 가능한 모든 보드)
- **환경:** HTML 파일을 실행할 로컬 서버 또는 최신 크롬 브라우저

---

## 2. 아두이노 코드 작성법 (프로토콜)

웹 GUI가 화면에 무엇을 그려야 할지 알 수 있도록, 아두이노는 **JSON 형식**의 문자열을 시리얼 포트로 전송해야 합니다.

### ① 컴포넌트 생성 (`init` 메시지)
아두이노가 시작될 때(`setup`) 또는 필요할 때 한 번만 전송합니다. 이 메시지를 받으면 웹 GUI에 해당 컨트롤러가 자동으로 생성됩니다.

**형식:** `{"type":"init", "id":"고유ID", "label":"표시이름", "kind":"종류", "min":최솟값, "max":최댓값}`

- **kind 종류:**
  - `toggle`: ON/OFF 스위치 (LED 제어 등에 사용)
  - `slider`: 값 조절 슬라이더 (밝기, 속도 제어 등에 사용)
  - `chart`: 실시간 선 그래프 (센서 값 모니터링에 사용)
- **예시:**
  ```cpp
  // LED 스위치 생성
  Serial.println(F("{\"type\":\"init\", \"id\":\"led1\", \"label\":\"거실 전등\", \"kind\":\"toggle\"}"));
  // 온도 센서 차트 생성
  Serial.println(F("{\"type\":\"init\", \"id\":\"temp1\", \"label\":\"현재 온도\", \"kind\":\"chart\", \"min\":0, \"max\":100}"));
  ```

### ② 데이터 업데이트 (`data` 메시지)
이미 생성된 컴포넌트의 값을 변경하고 싶을 때 사용합니다. `loop` 문 안에서 주기적으로 전송합니다.

**형식:** `{"type":"data", "id":"고유ID", "val":현재값}`

- **예시:**
  ```cpp
  int sensorVal = analogRead(A0);
  Serial.print(F("{\"type\":\"data\", \"id\":\"temp1\", \"val\":"));
  Serial.print(sensorVal);
  Serial.println(F("}"));
  ```

### ③ 웹에서 오는 명령 처리 (수신)
웹 GUI에서 스위치를 켜거나 슬라이더를 움직이면 아두이노로 JSON 데이터가 전송됩니다.

**수신 형식:** `{"id":"고유ID", "val":값}`

- **처리 방법:** `Serial.readStringUntil('\n')`로 읽어온 후 `id`와 `val` 값을 확인하여 `digitalWrite` 또는 `analogWrite`를 수행합니다.

---

## 3. 연결 및 실행 순서

1. **아두이노 코드 업로드:** 위 프로토콜에 맞게 작성된 코드를 아두이노 보드에 업로드합니다.
2. **HTML 파일 실행:** 제공된 `index.html` 파일을 브라우저로 엽니다.
3. **보드레이트(Baud rate) 설정:** 아두이노 코드의 `Serial.begin(9600)`과 웹 화면의 Baud rate 설정값이 일치하는지 확인합니다.
4. **연결 버튼 클릭:** `Connect Arduino` 버튼을 누르고, 팝업창에서 연결된 아두이노 포트를 선택합니다.
5. **확인:** 아두이노가 `init` 메시지를 보내면 화면에 자동으로 버튼과 차트가 나타납니다.

---

## 4. 주의사항 및 트러블슈팅

### ⚠️ 보안 제약 (Secure Context)
Web Serial API는 보안상의 이유로 **HTTPS** 환경이나 **localhost**에서만 작동합니다. 
- 단순 파일 실행(`file:///...`)으로 안 될 경우, VS Code의 **Live Server** 확장 프로그램을 사용하여 실행하세요.

### ⚠️ JSON 문법 주의
- JSON은 큰따옴표(`"`)를 사용해야 합니다. 아두이노 코드 내에서 문자열로 보낼 때는 `\"`와 같이 이스케이프 문자를 사용해야 합니다.
- `F()` 매크로를 사용하여 문자열을 플래시 메모리에 저장하면 RAM 부족 현상을 방지할 수 있습니다. (예: `Serial.println(F("..."));`)

### ⚠️ 데이터 전송 주기
- 너무 빠른 속도(예: 1ms 마다 전송)로 데이터를 보내면 웹 브라우저의 렌더링 속도가 따라가지 못해 렉이 발생할 수 있습니다. **100ms(0.1초) 정도의 간격**을 권장합니다.

---

## 5. 요약 표

| 기능 | 전송 방향 | 메시지 타입 | 핵심 키워드 | 용도 |
| :--- | :---: | :---: | :---: | :--- |
| **UI 생성** | 아두이노 $\rightarrow$ 웹 | `init` | `kind`, `label` | 버튼/차트 자동 생성 |
| **값 업데이트** | 아두이노 $\rightarrow$ 웹 | `data` | `id`, `val` | 실시간 수치 표시/그래프 업데이트 |
| **제어 명령** | 웹 $\rightarrow$ 아두이노 | `CMD` | `id`, `val` | LED 켜기, 모터 속도 조절 등 |

---
---

# 🇺🇸 [Guide] Arduino Web Serial GUI Monitor Usage

This system is a dynamic dashboard where the web browser interprets specific messages sent by the Arduino to **automatically render UI components** (sliders, toggles, charts, etc.) in real-time.

## 1. Prerequisites
- **Browser:** Google Chrome or Microsoft Edge (Latest version supporting Web Serial API).
  - *Note: Not supported in Firefox or Safari.*
- **Hardware:** Any Arduino-compatible board (Uno, Nano, Mega, ESP32, etc.).
- **Environment:** A local server or a modern Chrome browser to run the HTML file.

---

## 2. Arduino Coding Guide (Protocol)

To let the Web GUI know what to draw, the Arduino must send strings in **JSON format** via the serial port.

### ① Component Creation (`init` message)
Send this once during `setup()` or when a new component is needed. This tells the browser to create the widget.

**Format:** `{"type":"init", "id":"UniqueID", "label":"DisplayName", "kind":"Type", "min":MinValue, "max":MaxValue}`

- **Kind Types:**
  - `toggle`: ON/OFF switch (Used for LED control, etc.)
  - `slider`: Value adjustment slider (Used for brightness, speed, etc.)
  - `chart`: Real-time line graph (Used for sensor monitoring)
- **Example:**
  ```cpp
  // Create an LED switch
  Serial.println(F("{\"type\":\"init\", \"id\":\"led1\", \"label\":\"Living Room Light\", \"kind\":\"toggle\"}"));
  // Create a Temperature sensor chart
  Serial.println(F("{\"type\":\"init\", \"id\":\"temp1\", \"label\":\"Temperature\", \"kind\":\"chart\", \"min\":0, \"max\":100}"));
  ```

### ② Data Update (`data` message)
Use this to update the value of an existing component. Send this periodically inside the `loop()`.

**Format:** `{"type":"data", "id":"UniqueID", "val":CurrentValue}`

- **Example:**
  ```cpp
  int sensorVal = analogRead(A0);
  Serial.print(F("{\"type\":\"data\", \"id\":\"temp1\", \"val\":"));
  Serial.print(sensorVal);
  Serial.println(F("}"));
  ```

### ③ Handling Incoming Commands (Reception)
When a user toggles a switch or moves a slider on the Web GUI, a JSON packet is sent to the Arduino.

**Reception Format:** `{"id":"UniqueID", "val":Value}`

- **Handling:** Use `Serial.readStringUntil('\n')` to read the string and check the `id` and `val` to trigger `digitalWrite` or `analogWrite`.

---

## 3. Connection & Execution Steps

1. **Upload Arduino Code:** Upload the code following the protocol above to your board.
2. **Open HTML File:** Open the provided `index.html` file in your browser.
3. **Set Baud Rate:** Ensure the Baud rate on the web screen matches your Arduino code (e.g., `Serial.begin(9600)`).
4. **Click Connect:** Press the `Connect Arduino` button and select the correct COM port from the popup.
5. **Verify:** As soon as the Arduino sends the `init` messages, the buttons and charts will automatically appear on the screen.

---

## 4. Precautions & Troubleshooting

### ⚠️ Security Constraints (Secure Context)
The Web Serial API only works in **HTTPS** environments or **localhost** for security reasons.
- If opening the file directly (`file:///...`) doesn't work, use the **Live Server** extension in VS Code.

### ⚠️ JSON Syntax Accuracy
- JSON requires double quotes (`"`). In Arduino code, use escape characters (`\"`) when defining strings.
- Use the `F()` macro (e.g., `Serial.println(F("..."));`) to store strings in flash memory and prevent RAM exhaustion.

### ⚠️ Data Transmission Frequency
- Sending data too rapidly (e.g., every 1ms) may cause browser lag. An interval of **100ms (0.1s)** is recommended.

---

## 5. Summary Table

| Feature | Direction | Msg Type | Key Keywords | Purpose |
| :--- | :---: | :---: | :---: | :--- |
| **UI Creation** | Arduino $\rightarrow$ Web | `init` | `kind`, `label` | Auto-generate buttons/charts |
| **Value Update** | Arduino $\rightarrow$ Web | `data` | `id`, `val` | Real-time value display/graphing |
| **Control Cmd** | Web $\rightarrow$ Arduino | `CMD` | `id`, `val` | Turning on LEDs, adjusting motor speed, etc. |
