#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN         9
#define SS_PIN          10  //就是模組上的SDA接腳

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
double Body_temp = 0.00; //宣告個變數 存放體溫
MFRC522 mfrc522;   // 建立MFRC522實體
LiquidCrystal_I2C lcd(0x27, 20, 4); //建立LED  20字元4列

void setup() {
  Serial.begin(9600);
  SPI.begin();        // 初始化SPI介面
  mfrc522.PCD_Init(SS_PIN, RST_PIN); // 初始化MFRC522卡
  Serial.print(F("Reader "));
  Serial.print(F(": "));
  mfrc522.PCD_DumpVersionToSerial(); // 顯示讀卡設備的版本
  mlx.begin();
  pinMode(7, OUTPUT); //蜂鳴器正極 pin7
  lcd.init();
}

void loop() {
  // 檢查是不是一張新的卡
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // 顯示卡片內容
    lcd.init();                      // initialize the lcd
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("CardUID:");
    lcd.print(dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size));
    
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.print(F("PICC type: "));
    digitalWrite(7, HIGH);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));  //顯示卡片的類型

    Body_temp = (mlx.readAmbientTempC() * 0.16 + mlx.readObjectTempC()) - 1; //體溫補償
    lcd.setCursor(0, 1);
    lcd.print("Body_temp=" + String(Body_temp));
    lcd.print((char)0xDF); //符號
    lcd.print("C"); //符號
    mfrc522.PICC_HaltA(); // 卡片進入停止模式
    digitalWrite(7, LOW);
  }
}

/**
   這個副程式把讀取到的UID，用16進位顯示出來

  void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i], HEX);
    Serial.println();
  }
  }
*/

String dump_byte_array(byte *buffer, byte bufferSize) {
  String mystr = "";
  for (byte i = 0; i < bufferSize; i++) {
    mystr = String(buffer[i], HEX) + mystr;
  }
  mystr.toUpperCase();
  return mystr;
}


