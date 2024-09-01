//Include cac thu vien
#define DEFAULT_MQTT_HOST "mqtt1.eoh.io"
#define ERA_AUTH_TOKEN "ab6590bf-024e-4b2c-8c6e-3ed26a5fcce9"

#include <SPI.h>
#include <MFRC522.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>                                            
#include <Ticker.h> 
#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>

//Define Cam bien
#define SS_PIN    21  
#define RST_PIN   22  
#define DHTPIN    4     
#define DHTTYPE   DHT11
#define MQ2_PIN   34 
#define SDA_PIN   32   
#define SCL_PIN   33   



//Define Cua va Den
#define Khoa_Cua  2
#define Den_1     15
#define Den_2     12
#define Den_3     14
#define Den_4     27
#define Den_5     26
#define Chuong    5
#define Motor     3
#define Fan       1

/*PIN Ẩn
#define SDA       21
#define SCK       18
#define MOSI      23
#define MISO      19
#define RST       22
*/




// Khoi tao cac gia tri
LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 rfid(SS_PIN, RST_PIN);
DHT dht(DHTPIN, DHTTYPE);
Ticker LcdUpdateTimer;
Ticker buzzerTimer;
Ticker delayTimer;

void mainScreen();
void CheckRFID();
void Init();
void Warning();
void Buzzer();


float       humidity      = 0.0;
float       temperature   = 0.0;
int         SmokeValue    = 0;
int         TempSet       = 50;
int         SmokeSet      = 50;
const char  ssid[]        = "PhLu";
const char  pass[]        = "88888888";
int         check         =  0 ;
int         mode          =  0 ;
ERaTimer timer;


ERA_WRITE(V1)
{
  digitalWrite(Den_1 , param.getInt());
  if(param.getInt() == 1)
  {
    lcd.setCursor(3, 0);
    lcd.print("   ");
    lcd.setCursor(3, 0);
    lcd.print("ON");
  }
  else if(param.getInt() == 0)
  {
    lcd.setCursor(3, 0);
    lcd.print("   ");
    lcd.setCursor(3, 0);
    lcd.print("OFF");
  }
}

ERA_WRITE(V2)
{
  digitalWrite(Den_2 , param.getInt());
  if(param.getInt() == 1)
  {
    lcd.setCursor(10, 0);
    lcd.print("   ");
    lcd.setCursor(10, 0);
    lcd.print("ON");
  }
  else if(param.getInt() == 0)
  {
    lcd.setCursor(10, 0);
    lcd.print("   ");
    lcd.setCursor(10, 0);
    lcd.print("OFF");
  }
}

ERA_WRITE(V3)
{
  digitalWrite(Den_3 , param.getInt());
  if(param.getInt() == 1)
  {
    lcd.setCursor(17, 0);
    lcd.print("   ");
    lcd.setCursor(17, 0);
    lcd.print("ON");
  }
  else if(param.getInt() == 0)
  {
    lcd.setCursor(17, 0);
    lcd.print("   ");
    lcd.setCursor(17, 0);
    lcd.print("OFF");
  }
}

ERA_WRITE(V4)
{
  digitalWrite(Den_4 , param.getInt());
  if(param.getInt() == 1)
  {
    lcd.setCursor(3, 1);
    lcd.print("   ");
    lcd.setCursor(3, 1);
    lcd.print("ON");
  }
  else if(param.getInt() == 0)
  {
    lcd.setCursor(3, 1);
    lcd.print("   ");
    lcd.setCursor(3, 1);
    lcd.print("OFF");
  }
}

ERA_WRITE(V5)
{
  digitalWrite(Den_5 , param.getInt());
  if(param.getInt() == 1)
  {
    lcd.setCursor(10, 1);
    lcd.print("   ");
    lcd.setCursor(10, 1);
    lcd.print("ON");
  }
  else if(param.getInt() == 0)
  {
    lcd.setCursor(10, 1);
    lcd.print("   ");
    lcd.setCursor(10, 1);
    lcd.print("OFF");
  }
}


ERA_WRITE(V9)
{
  TempSet = param.getInt();
}
ERA_WRITE(V10)
{
  SmokeSet = param.getInt();
}



ERA_WRITE(V11)
{
  if(param.getInt() == 1)
  {
    lcd.clear();
    while(param.getInt() == 1);
    mode++;
    mode = mode % 3;
  }
}


void timerEvent() 
{
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}






//SETUP
void setup() 
{
  Serial.begin(9600);

  ERa.begin(ssid, pass);
  timer.setInterval(1000L, timerEvent);

  Init();

  LcdUpdateTimer.attach(1, mainScreen);
}





//LOOP
void loop() 
{
  ERa.run();
  timer.run();
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) 
  {
    if (rfid.uid.uidByte[0] == 0xD3 && rfid.uid.uidByte[1] == 0x94) 
    {
      CheckRFID();
      digitalWrite(Chuong , HIGH);
      buzzerTimer.attach(0.2, Buzzer);
      ERa.virtualWrite(V1,HIGH);
      ERa.virtualWrite(V2,HIGH);
      ERa.virtualWrite(V3,HIGH);
      ERa.virtualWrite(V4,HIGH);
      ERa.virtualWrite(V5,HIGH);
    } 
  }

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);

  SmokeValue = ((analogRead(MQ2_PIN) * 100) / 4095);
  Serial.print("Smoke Value: ");
  Serial.println(SmokeValue);

  Warning();
  ERa.virtualWrite(V6,SmokeValue);
  ERa.virtualWrite(V7,humidity);
  ERa.virtualWrite(V8,temperature);
}





//Ham LCD
void mainScreen() 
{
  if(mode == 0)
  {
  lcd.setCursor(6, 2);
  lcd.print("   ");
  lcd.setCursor(16, 2);
  lcd.print("   ");
  lcd.setCursor(15, 3);
  lcd.print("   ");

  lcd.setCursor(0, 0);  // Line 0, column 0
  lcd.print("L1:");
  lcd.setCursor(3, 0);
  if (digitalRead(Den_1) == HIGH) 
  {
    lcd.print("ON");
  }
  else 
  {
    lcd.print("OFF");
  }

  lcd.setCursor(7, 0);  // Line 0, column 7
  lcd.print("L2:");
  lcd.setCursor(10, 0);
  if (digitalRead(Den_2) == HIGH) 
  {
    lcd.print("ON");
  }
  else 
  {
    lcd.print("OFF");
  }

  lcd.setCursor(14, 0); // Line 0, column 14
  lcd.print("L3:");
  lcd.setCursor(17, 0);
  if (digitalRead(Den_3) == HIGH) 
  {
    lcd.print("ON");
  }
  else 
  {
    lcd.print("OFF");
  }

  lcd.setCursor(0, 1);  // Line 1, column 0
  lcd.print("L4:");
  lcd.setCursor(3, 1);
  if (digitalRead(Den_4) == HIGH) 
  {
    lcd.print("ON");
  }
  else 
  {
    lcd.print("OFF");
  }

  lcd.setCursor(7, 1);  // Line 1, column 7
  lcd.print("L5:");
  lcd.setCursor(10, 1);
  if (digitalRead(Den_5) == HIGH) 
  {
    lcd.print("ON");
  }
  else 
  {
    lcd.print("OFF");
  }


  lcd.setCursor(0, 2);  // Line 2, column 0
  lcd.print("Smoke:");
  lcd.print(SmokeValue);

  lcd.setCursor(10, 2); // Line 2, column 10
  lcd.print("Humid:");
  lcd.print((int)humidity);  // Print humidity with 1 decimal place

  lcd.setCursor(10, 3); // Line 3, column 10
  lcd.print("Temp:");
  lcd.print((int)temperature);  // Print temperature with 1 decimal place
  }


  if(mode == 1)
  {
  lcd.setCursor(0, 0);
  lcd.print("Smoke:");
  lcd.print((int)SmokeValue);
  lcd.setCursor(0, 1);
  lcd.print("Set:");
  lcd.setCursor(4, 1);
  lcd.print("    ");
  lcd.print((int)SmokeSet);
  }

  if(mode == 2)
  {
  lcd.setCursor(0, 0);
  lcd.print("Temper:");
  lcd.print((int)temperature);
  lcd.setCursor(0, 1);
  lcd.print("Set:");
  lcd.setCursor(4, 1);
  lcd.print("    ");
  lcd.print((int)TempSet);
  }
}



  



void Init() {

  SPI.begin(18, 19, 23, SS_PIN);  // SCK=18, MISO=19, MOSI=23, SS=21
  dht.begin();
  rfid.PCD_Init();
  
  pinMode(Khoa_Cua, OUTPUT);
  pinMode(Den_1, OUTPUT);
  pinMode(Den_2, OUTPUT);
  pinMode(Den_3, OUTPUT);
  pinMode(Den_4, OUTPUT);
  pinMode(Den_5, OUTPUT);
  pinMode(Chuong,OUTPUT);
  pinMode(Motor, OUTPUT);
  pinMode(Fan,   OUTPUT);



  digitalWrite(Khoa_Cua, LOW);
  digitalWrite(Den_1, LOW);
  digitalWrite(Den_2, LOW);
  digitalWrite(Den_3, LOW);
  digitalWrite(Den_4, LOW);
  digitalWrite(Den_5, LOW);
  digitalWrite(Motor, LOW);
  digitalWrite(Fan, LOW);
  digitalWrite(Chuong, LOW);

  ERa.virtualWrite(V1,LOW);
  ERa.virtualWrite(V2,LOW);
  ERa.virtualWrite(V3,LOW);
  ERa.virtualWrite(V4,LOW);
  ERa.virtualWrite(V5,LOW);


  pinMode(MQ2_PIN, INPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.clear();
  //LCD_ScreenInit
  lcd.setCursor(3, 0);  // Line 0, column 0
  lcd.print("OFF");

  lcd.setCursor(10, 0);  // Line 0, column 7
  lcd.print("OFF");

  lcd.setCursor(17, 0); // Line 0, column 14
  lcd.print("OFF");

  lcd.setCursor(3, 1);  // Line 1, column 0
  lcd.print("OFF");

  lcd.setCursor(10, 1);  // Line 1, column 7
  lcd.print("OFF");

}






//Ham Khoa cua va bat den
void CheckRFID()
{
    digitalWrite(Khoa_Cua, HIGH);
    delay(2000);
    digitalWrite(Khoa_Cua, LOW);
    digitalWrite(Den_1, HIGH);
    digitalWrite(Den_2, HIGH);
    digitalWrite(Den_3, HIGH);
    digitalWrite(Den_4, HIGH);
    digitalWrite(Den_5, HIGH);

    lcd.setCursor(3, 0);  // Line 0, column 0
    lcd.print("   ");

    lcd.setCursor(10, 0);  // Line 0, column 7
    lcd.print("   ");

    lcd.setCursor(17, 0); // Line 0, column 14
    lcd.print("   ");

    lcd.setCursor(3, 1);  // Line 1, column 0
    lcd.print("   ");

    lcd.setCursor(10, 1);  // Line 1, column 7
    lcd.print("   ");

    lcd.setCursor(3, 0);  // Line 0, column 7
    lcd.print("ON");

    lcd.setCursor(10, 0);  // Line 0, column 7
    lcd.print("ON");

    lcd.setCursor(17, 0); // Line 0, column 14
    lcd.print("ON");

    lcd.setCursor(3, 1);  // Line 1, column 0
    lcd.print("ON");

    lcd.setCursor(10, 1);  // Line 1, column 7
    lcd.print("ON");
}






void Warning()
{
    if(temperature > TempSet)
  {
    digitalWrite(Chuong , HIGH);
    digitalWrite(Motor, HIGH);
  }
  else if(SmokeValue > SmokeSet)
  {
    digitalWrite(Chuong , HIGH);
    digitalWrite(Fan, HIGH);
  }
  else
  {
    digitalWrite(Chuong , LOW);
    digitalWrite(Motor, LOW);
    digitalWrite(Fan, LOW);
  }
}




void Buzzer() 
{
  digitalWrite(Chuong, LOW);
}
