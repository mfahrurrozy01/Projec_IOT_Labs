# All of ToT project
# 
# ESP8266 30 Pin NodeMCU PIN-OUT
![Wiring](https://github.com/NugrohoESBB/projectToT/blob/main/esp8266-nodemcu-pinout.jpg)

# 

## 1. Irrigation System
Run Website -> not available

## Schematic
![Wiring](https://github.com/NugrohoESBB/projectToT/blob/main/irrigationSystem/Wiring%20Irrigation%20System.jpg)

## Documentation
## Soil Moisture Sensor Pin Use to ESP8266 NodeMCU

| PIN Soil | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `DATA` | `Communication` |`A0` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `3.3V`|

## Relay 1 Channel Pin Use to ESP8266 NodeMCU

| PIN Relay | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `IN` | `Communication` |`D1 (GPIO5)` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `3.3V`|

## Water Pump & Battery Pin Use to Relay 1 Channel

| PIN Water Pump & Battery | Type     | Pin Relay & Battery| 
| :-------- | :------- |  :------- |
| `VCC` | `Power` | `NO`|
| `VCC Battery` | `Power` | `COM`|
| `GND` | `Ground` |`GND Battery`|

## Buzzer Pin Use to ESP8266 NodeMCU

| PIN Buzzer | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `VCC` | `Power & Communication` | `D2 (GPIO4)`|
| `GND` | `Ground` |`GND`|

## Buzzer Pin Use to WeMos D1 Mini (3 Pin Version)

| PIN Buzzer | Type     | Pin WeMos D1 Mini| 
| :-------- | :------- |  :------- |
| `DATA` | `Communication` |`D2 (GPIO4)` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `5V`|

## LED Traffic Light Pin Use to ESP8266 NodeMCU

| PIN LED | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `Red` | `Power & Communication` | `D5 (GPIO14)`|
| `Yellow` | `Power & Communication` | `D6 (GPIO12)`|
| `Green` | `Power & Communication` | `D7 (GPIO13)`|
| `GND` | `Ground` |`GND`|

## Formula for reading the soil moisture sensor as a percentage
```c++
int readSoilPercent() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(PIN_SOIL);
    delay(5);
  }
  int raw = sum / 10;
  int pct = map(raw, SOIL_DRY, SOIL_WET, 0, 100);
  return constrain(pct, 0, 100);
}
```

# 

## 2. Hydroponic Water Level System
Run Website -> not available

## Schematic
![Wiring](https://github.com/NugrohoESBB/projectToT/blob/main/hydroponicWaterLevel/Wiring%20Hydroponic%20Water%20Level.jpg)

## Documentation
## Water Level Sensor Pin Use to ESP8266 NodeMCU

| PIN Water Level | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `DATA` | `Communication` |`A0` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `D0 (GPIO16)`|

## Relay 1 Channel Pin Use to ESP8266 NodeMCU

| PIN Relay | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `IN` | `Communication` |`D1 (GPIO5)` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `3.3V`|

## Water Pump & Battery Pin Use to Relay 1 Channel

| PIN Water Pump & Battery | Type     | Pin Relay & Battery| 
| :-------- | :------- |  :------- |
| `VCC` | `Power` | `NO`|
| `VCC Battery` | `Power` | `COM`|
| `GND` | `Ground` |`GND Battery`|

## Buzzer Pin Use to ESP8266 NodeMCU

| PIN Buzzer | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `VCC` | `Power & Communication` | `D2 (GPIO4)`|
| `GND` | `Ground` |`GND`|

## Buzzer Pin Use to WeMos D1 Mini (3 Pin Version)

| PIN Buzzer | Type     | Pin WeMos D1 Mini| 
| :-------- | :------- |  :------- |
| `DATA` | `Communication` |`D2 (GPIO4)` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `5V`|

## LED Traffic Light Pin Use to ESP8266 NodeMCU

| PIN LED | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `Red` | `Power & Communication` | `D5 (GPIO14)`|
| `Yellow` | `Power & Communication` | `D6 (GPIO12)`|
| `Green` | `Power & Communication` | `D7 (GPIO13)`|
| `GND` | `Ground` |`GND`|

## Formula for reading the water level as a percentage
```c++
int readLevelPercent() {
  digitalWrite(PIN_SENSOR_PWR, HIGH);
  delay(50);
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(PIN_LEVEL);
    delay(5);
  }
  digitalWrite(PIN_SENSOR_PWR, LOW);
  int raw = sum / 10;
  int pct = map(raw, LEVEL_EMPTY, LEVEL_FULL, 0, 100);
  return constrain(pct, 0, 100);
}
```

# 

## 3. Parking Gate System
Run Website -> not available

## Schematic
![Wiring](https://github.com/NugrohoESBB/projectToT/blob/main/parkingGate/parking%20system.png)

## Schematic (Powerbank Version)
![Wiring](https://github.com/NugrohoESBB/projectToT/blob/main/parkingGate/parking%20system%20(powerbank%20version).png)

## Documentation
## HC-SR04 (Ultrasonic Sensor) Pin Use to ESP8266 NodeMCU

| PIN Water Level | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `ECHO` | `Communication` |`D2 (GPIO4)` |
| `TRIG` | `Communication` |`D1 (GPIO5)` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `5V`|

## Servo Pin Use to ESP8266 NodeMCU

| PIN Relay | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `DATA (Orange)` | `Communication` |`D5 (GPIO14)` |
| `GND (Brown)` | `Ground` |`GND`|
| `VCC (Red)` | `Power` | `5V`|

## Formula for reading the distance in cm from the ultrasonic sensor
```c++
float readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000);
  if (duration == 0) {
    return 999.0;
  }
  return duration * 0.0343 / 2.0;
}
```

# 

## 4. Parking Slot Detection System
Run Website -> not available

## Schematic
![Wiring](https://github.com/NugrohoESBB/projectToT/blob/main/parkingSlot/parking%20slot%20detection.png)

## Schematic (Powerbank Version)
![Wiring](https://github.com/NugrohoESBB/projectToT/blob/main/parkingSlot/parking%20slot%20detection%20(powerbank%20version).png)

## Documentation
## IR Sensor Pin Use to ESP8266 NodeMCU

| PIN Water Level | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `DATA` | `Communication` |`D5 (GPIO14)` |
| `GND` | `Ground` |`GND`|
| `VCC` | `Power` | `3.3V or 5V`|

## LCD 16x2 with I2C Pin Use to ESP8266 NodeMCU

| PIN LCD | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `SDA` | `Communication Serial` |`D2 (SDA)` |
| `SCL` | `Communication Serial` |`D1 (SCL)`|
| `GND` | `Ground` |`GND`|
| `5V` | `VCC` | `5V`|

## LED Traffic Light Pin Use to ESP8266 NodeMCU

| PIN LED | Type     | Pin ESP8266 NodeMCU| 
| :-------- | :------- |  :------- |
| `Red` | `Power & Communication` | `D6 (GPIO12)`|
| `Yellow` | `Power & Communication` | `D7 (GPIO13)`|
| `Green` | `Power & Communication` | `D0 (GPIO16)`|
| `GND` | `Ground` |`GND`|

# 

## NUBARA © Nugroho Eko S Batubara 2020 - 2026. All Rights Reserved.

# 

