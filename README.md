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
