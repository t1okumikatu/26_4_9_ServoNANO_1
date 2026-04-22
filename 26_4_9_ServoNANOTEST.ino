#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

uint8_t servoIn  = 0; // 入口
uint8_t servoOut = 1; // 出口

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);

  Serial.println("サーボテスト開始");
  Serial.println("例: 1500 と入力するとその位置に動く");
}

void loop() {
  if (Serial.available() > 0) {
    int us = Serial.parseInt();

    if (us > 400 && us < 2600) {
      Serial.print("移動: ");
      Serial.println(us);

      pwm.writeMicroseconds(servoIn, us);
      pwm.writeMicroseconds(servoOut, us);
    } else {
      Serial.println("範囲外（500〜2500くらいにして）");
    }
  }
}