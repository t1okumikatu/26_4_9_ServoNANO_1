#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
//#define USMIN  600    USMAX  2400
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// servo # counter : IN_Servo/14, OUT_Servo/15
uint8_t servoIn  = 0;
uint8_t servoOut = 1;

const int digitalPin1 = 2; // 入口Sub
const int digitalPin2 = 3; // 入口Main
const int digitalPin3 = 4; // 出口Main
const int digitalPin4 = 5; // 出口Sub
int val1 = HIGH;  // 制御ピン（In側Sub）
int val2 = HIGH;  // 制御ピン（In側Main）
int val3 = HIGH;  // 制御ピン（Out側Main)
int val4 = HIGH;  // 制御ピン（Out側Sub）
int val1_FLG = 0; // 前回設定と同一の場合、無視する
int val2_FLG = 0; // 前回設定と同一の場合、無視する
int val3_FLG = 0; // 前回設定と同一の場合、無視する
int val4_FLG = 0; // 前回設定と同一の場合、無視する
/*
int USMIN_In =  965; // In側Main側角度
int USMAX_In = 1080; // In側Sub側角度
int USMIN_Out = 1000;// Out側Sub側角度
int USMAX_Out = 1160;// Out側Main側角度
*/
int USMIN_In =  495; // In側Main側角度
int USMAX_In =  505; // In側Sub側角度
int USMIN_Out = 495;// Out側Sub側角度
int USMAX_Out = 505;// Out側Main側角度
uint16_t microsec1;  // 制御スピード
uint16_t microsec2;  //
uint16_t microsec3;  //
uint16_t microsec4;  //
int MIN_In = 1;      // 現状角度位置フラグ
int MAX_In = 1;      //　　制御が同一なら機能させない
int MIN_Out = 1;     //　　異なる場合のみ機能させる
int MAX_Out = 1;     //

void setup() {
  Serial.begin(9600);
  pinMode(digitalPin1, INPUT_PULLUP); // digitalPin(2番ピン)
  pinMode(digitalPin2, INPUT_PULLUP); // digitalPin(3番ピン)
  pinMode(digitalPin3, INPUT_PULLUP); // digitalPin(4番ピン)
  pinMode(digitalPin4, INPUT_PULLUP); // digitalPin(5番ピン)
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  delay(100);
  pwm.setPWMFreq(SERVO_FREQ);// Analog servos run at ~50 Hz updates
  delay(10);
  //pwm.writeMicroseconds(servoIn, 1010);  // Minポジション
  pwm.writeMicroseconds(servoIn, 500);  // Minポジション
  delay(1000);
  //pwm.writeMicroseconds(servoOut, 1010); // Minポジション
  pwm.writeMicroseconds(servoOut, 500); // Minポジション
  delay(1000);
  MIN_In = 0;   // In側サーボは、Main位置にいる。
  MAX_In = 1;   // IN側サーボは、Sub位置にいる。
  MIN_Out = 1;  // Out側サーボは、Sub位置にいる。
  MAX_Out = 0;  // Out側サーボは、Main位置にいる。
  while(!(val1==HIGH && val2==LOW && val3==LOW && val4==HIGH)){
    val1 = digitalRead(digitalPin1);  // digitalPin1の状態を読み取る。
    val2 = digitalRead(digitalPin2);  // digitalPin2の状態を読み取る。
    val3 = digitalRead(digitalPin3);  // digitalPin3の状態を読み取る。
    val4 = digitalRead(digitalPin4);  // digitalPin4の状態を読み取る。
  }
}

void loop() {
//  Serial.println("0");
  val1_FLG = 0;
  val2_FLG = 0;
  val3_FLG = 0;
  val4_FLG = 0;
  val1 = digitalRead(digitalPin1);  // digitalPin1の状態を読み取る。
  val2 = digitalRead(digitalPin2);  // digitalPin2の状態を読み取る。
  val3 = digitalRead(digitalPin3);  // digitalPin3の状態を読み取る。
  val4 = digitalRead(digitalPin4);  // digitalPin4の状態を読み取る。
//  Serial.println("1");
  if(val1 == LOW && val1_FLG == 0 && MIN_In == 1){
    val1_FLG = 1;
    for (microsec1 = USMIN_In; microsec1 < USMAX_In; microsec1++) {
      pwm.writeMicroseconds(servoIn, microsec1);
    }
//    while(val1 == LOW){
//      val1 = digitalRead(digitalPin1);
//     }
     MIN_In = 0;
     MAX_In = 1;
   }
//   Serial.println("2");
  if(val2 == LOW && val2_FLG == 0 && MAX_In == 1){
    val2_FLG = 1;
     for (microsec2 = USMAX_In; microsec2 > USMIN_In; microsec2--) {
     pwm.writeMicroseconds(servoIn, microsec2);
     }
//     while(val2 == LOW){
//      val2 = digitalRead(digitalPin2);
//     }
     MIN_In = 1;
     MAX_In = 0;
   }
//   Serial.println("3");
  if(val3 == LOW && val3_FLG == 0 && MIN_Out == 1){
    val3_FLG = 1;
//    Serial.println("3_1");
     for (microsec3 = USMIN_Out; microsec3 < USMAX_Out; microsec3++) {
      pwm.writeMicroseconds(servoOut, microsec3);
     }
//     Serial.println("3_2");
//     while(val3 == LOW){
//      val3 = digitalRead(digitalPin3);
//      Serial.print("val3 : ");
//      Serial.println(val3);
//     }
     MIN_Out = 0;
     MAX_Out = 1;
//     Serial.println("3_4");
   }
//   Serial.println("4");
  if(val4 == LOW && val4_FLG == 0 && MAX_Out == 1){
     val4_FLG = 1;
     for (microsec4 = USMAX_Out; microsec4 > USMIN_Out; microsec4--) {
     pwm.writeMicroseconds(servoOut, microsec4);
     }
//     while(val4 == LOW){
//      val4 = digitalRead(digitalPin4);
//     }
     MIN_Out = 1;
     MAX_Out = 0;
   }
//   Serial.println("5");
   delay(100);
}
