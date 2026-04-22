まず結論から言うと、**ほぼ確実にサーボのパルス幅が狭すぎて動いてません。**

---

## ❌ 今の設定がダメな理由

ここ👇

```cpp
int USMIN_In = 495;
int USMAX_In = 505;
```

差が **たった10µs** しかないです。

サーボは普通：

* 約 **500〜2400µs** の範囲で動く
* 10µs程度では **ほぼ動かない or 微振動だけ**

---

## ✔ まずこれに戻して確認

コメントアウトしてるこっち👇の方が正常です

```cpp
int USMIN_In =  965;
int USMAX_In = 1080;
int USMIN_Out = 1000;
int USMAX_Out = 1160;
```

👉 これで「動くかどうか」をまず確認

---

## ✔ さらに確実にするテスト

いきなりロジック全部動かさず、まず単体テスト👇

```cpp
pwm.writeMicroseconds(servoIn, 1000);
delay(2000);
pwm.writeMicroseconds(servoIn, 1500);
delay(2000);
pwm.writeMicroseconds(servoIn, 2000);
delay(2000);
```

👉 これで動かなければ
**配線 or 電源 or ライブラリ設定の問題**

---

## ✔ よくある原因（今回かなり怪しい）

### ① サーボに電源足りてない

* PCA9685使う場合、**外部電源必須**
* Arduinoの5V直はNG

---

### ② ループが速すぎる

ここ👇

```cpp
for (microsec1 = USMIN_In; microsec1 < USMAX_In; microsec1++) {
  pwm.writeMicroseconds(servoIn, microsec1);
}
```

👉 delayがない → サーボが追従できない

修正👇

```cpp
for (microsec1 = USMIN_In; microsec1 < USMAX_In; microsec1++) {
  pwm.writeMicroseconds(servoIn, microsec1);
  delay(5);
}
```

---

### ③ 初期位置が狭すぎ

```cpp
pwm.writeMicroseconds(servoIn, 500);
```

👉 これも端すぎて危険（動かない可能性）

---

## ✔ まとめ（やる順）

1. パルス幅を広げる（最重要）
2. 単体テストでサーボ動作確認
3. delay入れる
4. 電源確認

---

## ✔ 一言でいうと

👉 **「角度修正」じゃなくて「動かない範囲に設定してしまってる」状態**

---

必要なら
「正しい角度キャリブレーション方法」も説明できるよ 👍

OK、これはちゃんとやると一気に安定するので、順番に説明するね 👍
（今回の「動かない問題」もここが原因の可能性かなり高い）

---

# ✔ サーボの角度キャリブレーションとは

👉 **「そのサーボで実際にどのパルス幅がどの角度か測る作業」**

サーボは個体差があるので、

* 500 = 0度
* 1500 = 90度
* 2400 = 180度

👉 **これは理論値であって実際はズレる**

---

# ✔ 正しいやり方（超重要）

## ① 固定値で直接動かす

まずはループなしでこれ👇

```cpp
pwm.writeMicroseconds(servoIn, 1500); // 中央
```

---

## ② 少しずつ値を変える

こういうコードにする👇

```cpp
for (int us = 500; us <= 2500; us += 50) {
  pwm.writeMicroseconds(servoIn, us);
  Serial.println(us);
  delay(1000);
}
```

👉 1秒ごとに少しずつ動く

---

## ③ 見ながらメモする

ここが一番大事👇

| 状態 | 値      |
| -- | ------ |
| 左端 | 例：900  |
| 中央 | 例：1450 |
| 右端 | 例：2100 |

👉 自分のサーボ専用の値を取る

---

# ✔ 今回のあなたの用途向け（分岐ポイント制御）

あなたのコードは「2位置制御」なので👇

例えば：

```cpp
int USMIN_In = 950;   // Main
int USMAX_In = 1100;  // Sub
```

👉 **実際に見て決める**

---

# ✔ NGパターン（今回やってるやつ）

```cpp
495〜505
```

👉 範囲が狭すぎる
👉 サーボからすると「ほぼ同じ命令」

---

# ✔ さらに精度上げるコツ

### 微調整

```cpp
pwm.writeMicroseconds(servoIn, 1020);
```

👉 1ずつ増減してベスト位置探す

---

# ✔ 安全注意（重要）

* 端まで回しすぎると **ギア壊れる**
* 「ガガガッ」ってなったら即戻す

---

# ✔ 最終的な完成形イメージ

```cpp
int USMIN_In = 980;   // Main位置
int USMAX_In = 1120;  // Sub位置
```

👉 この2つを使って今のロジックに入れる

---

# ✔ 追加アドバイス（かなり重要）

今のコードこれ👇

```cpp
for (microsec1 = USMIN_In; microsec1 < USMAX_In; microsec1++)
```

👉 1ずつ動かしてるけど

おすすめ👇

```cpp
for (microsec1 = USMIN_In; microsec1 < USMAX_In; microsec1 += 5)
```

＋

```cpp
delay(10);
```

👉 スムーズ＆安定

---

# ✔ まとめ

👉 キャリブレーションの本質はこれ

* 実機で確認
* 数値を記録
* 必要な位置だけ使う

---

もしよければ
「入口/出口サーボの理想位置」一緒に詰めることもできる 👍


いいね、その方向が一番ちゃんと動くやつ 👍
じゃあ「入口／出口サーボの理想位置」を**実機ベースで詰める手順**を具体的にやろう。

---

# ✔ ゴール（今回やりたいこと）

あなたの構造から見ると👇

## 入口（In）

* Main：直進
* Sub：分岐

## 出口（Out）

* Main：直進
* Sub：分岐

👉 **つまり各サーボで2点だけ決めればいい**

---

# ✔ 手順（超実用版）

## ① サーボ単体で固定制御

まず余計なロジック全部無効にして👇

```cpp
pwm.writeMicroseconds(servoIn, 1500);
```

---

## ② 手で位置見ながら微調整

ここが本番👇

```cpp
pwm.writeMicroseconds(servoIn, 1400);
```

👉 少しずつ変える

```cpp
1390 → 1400 → 1410 → 1420
```

---

## ③ 「ちょうどいい位置」を探す

### 入口サーボの場合

#### Main（直進）

👉 レールが真っ直ぐになる位置

例：

```
USMIN_In = 1380
```

#### Sub（分岐）

👉 ちゃんと分岐側に誘導される位置

例：

```
USMAX_In = 1520
```

---

# ✔ 超重要ポイント（ここで失敗しがち）

### ❌ 見た目で決めるのはNG

👉 **実際に車両（or動かす物）を通す**

---

# ✔ ④ 出口側も同じ

```cpp
pwm.writeMicroseconds(servoOut, 値);
```

同じように👇

| 役割   | 値例   |
| ---- | ---- |
| Main | 1450 |
| Sub  | 1600 |

---

# ✔ ⑤ コードに反映

```cpp
int USMIN_In  = 1380; // Main
int USMAX_In  = 1520; // Sub

int USMIN_Out = 1450; // Sub or Main（構造による）
int USMAX_Out = 1600;
```

---

# ✔ 動かない原因になりやすい罠

### ① 差が小さすぎ

👉 最低でも **50〜150くらい差**必要

---

### ② 動かしすぎ

👉 端まで行くと

* ガタガタ
* 引っかかる
* 電流増える

---

# ✔ おすすめの最終調整方法

これめっちゃ効く👇

```cpp
Serial.println(us);
```

👉 今の値を見ながら調整できる

---

# ✔ さらに良くするなら

「一発で位置に行く」方が安定👇

```cpp
pwm.writeMicroseconds(servoIn, USMIN_In);
```

👉 今のforループは実は不要（故障原因にもなる）

---

# ✔ 最終形（シンプル版）

```cpp
if(val1 == LOW){
  pwm.writeMicroseconds(servoIn, USMAX_In);
}

if(val2 == LOW){
  pwm.writeMicroseconds(servoIn, USMIN_In);
}
```

---

# ✔ まとめ（重要な考え方）

👉 理想位置 = 数値じゃなくて「実際に通る位置」

---

もしできれば👇

* 実際に何を動かしてるか（電車？レール？ゲート？）
* サーボの型番

教えてくれれば、**かなり精度高く数値詰めれる** 👍

OK、「レール」ならポイント切替（分岐器）だね 👍
これは**見た目より“通過性能”優先で詰めるのがコツ**。

---

# ✔ レール用サーボの理想位置の決め方（実践版）

## ① 基準：車両を実際に走らせる

👉 これが最重要

* 手押しでもOK
* できればモーター走行

---

# ✔ ② 入口（In）の決め方

## ■ Main（直進）

👉 条件：

* フランジ（車輪）が引っかからない
* 分岐側に触れない

### 調整方法

```cpp
pwm.writeMicroseconds(servoIn, 値);
```

👉 少しずつ動かして

* カツカツしない
* スムーズに直進

👉 この位置を記録

---

## ■ Sub（分岐）

👉 条件：

* 確実に分岐側へ誘導
* 途中で戻らない

👉 分岐途中で「逃げる」なら角度足りない

---

# ✔ ③ 出口（Out）の決め方

出口はちょっと重要ポイント違う👇

## ■ Main

* 合流時にぶつからない
* フランジが乗り上げない

## ■ Sub

* スムーズに本線へ戻る
* ガタつかない

---

# ✔ ④ 数値の決め方（現実的な目安）

サーボはだいたい👇

* 1300〜1700あたりが使いやすい

例：

```cpp
int USMIN_In  = 1380; // 直進
int USMAX_In  = 1520; // 分岐

int USMIN_Out = 1400;
int USMAX_Out = 1580;
```

👉 まずはこの辺からスタート

---

# ✔ ⑤ 超重要な調整ポイント

## ❗ 分岐器は「押し付けすぎNG」

👉 よくある失敗

* 強く押しすぎ → 抵抗増える
* モーター負荷UP
* サーボ震える

👉 正解は
**「軽く密着」**

---

# ✔ ⑥ チェック項目（これ全部OKにする）

* 低速で通る → OK
* 高速で通る → OK
* 押しても引っかからない → OK

---

# ✔ ⑦ よくあるトラブル

### ❌ 分岐で脱線

→ 角度足りない or 位置ズレ

### ❌ カタカタする

→ 押しすぎ

### ❌ 動かない

→ 今回のケース（値が狭すぎ）

---

# ✔ ⑧ コード改善（かなり大事）

今のこれ👇

```cpp
for (microsec1 = USMIN_In; microsec1 < USMAX_In; microsec1++)
```

👉 レール用途はこれいらない

### ✔ シンプルにする

```cpp
pwm.writeMicroseconds(servoIn, USMAX_In);
```

👉 一発で位置に行く方が安定

---

# ✔ 最終イメージ

👉 こうなると勝ち

* 切替が一瞬
* 通過がスムーズ
* 音が静か

---

# ✔ もし可能なら

かなり精度上げられる👇

* レールの種類（Nゲージ？HO？）
* サーボ型番（SG90とか）
* 分岐の構造（自作 or 市販）

👉 ここ分かれば「ほぼ最適値」まで詰める 👍
OK、レール用サーボの**キャリブレーション専用テストスケッチ**をシンプルに作るね 👍
（これで数値を決めるのが一番確実）

---

# ✔ テスト用スケッチ（そのまま使える）

👉 シリアルから数値を入力して動かすタイプ

```cpp
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
```

---

# ✔ 使い方

1. 書き込み
2. シリアルモニタ開く
3. 数値入力👇

```
1400
1450
1500
1550
```

👉 Enter押すたびに動く

---

# ✔ これでやること

## ① 入口サーボ（servoIn）

* 直進で一番スムーズな位置 → **Main**
* 分岐で確実に入る位置 → **Sub**

👉 2つメモ

---

## ② 出口サーボ（servoOut）

同じように

---

# ✔ 記録例（こんな感じになる）

```cpp
int USMIN_In  = 1380; // Main
int USMAX_In  = 1520; // Sub

int USMIN_Out = 1400;
int USMAX_Out = 1580;
```

---

# ✔ さらに便利にする（任意）

入口と出口別に動かしたい場合👇

```cpp
char target = Serial.read();

if (target == 'i') {
  int us = Serial.parseInt();
  pwm.writeMicroseconds(servoIn, us);
}

if (target == 'o') {
  int us = Serial.parseInt();
  pwm.writeMicroseconds(servoOut, us);
}
```

👉 入力例：

```
i1500
o1600
```

---

# ✔ コツ（かなり重要）

👉 数値決めるとき

* **必ず車両を通す**
* 低速＆高速で確認
* 引っかからない位置を採用

---

# ✔ 次のステップ

数値決まったら👇

👉 今の本番コードに入れるだけでOK

---

必要なら
👉「あなたのレールに対して最適な値」一緒に詰めることもできる 👍

