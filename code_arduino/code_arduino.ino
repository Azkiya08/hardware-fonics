#include "GravityTDS.h"
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>
#include "GravityTDS.h"


RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
#define detik now.second()
#define menit now.minute()
#define jam now.hour()
#define tanggal now.day()
#define hari daysOfTheWeek[now.dayOfTheWeek()]
#define bulan now.month()
#define tahun now.year()

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial serialtoESP(3, 4);  // RX, TX

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


#define TDS_SENSOR A0
//#define EEPROM_SIZE 512
//GravityTDS gravityTds;
//float temperature = 25, tdsValue = 0;
float tdsValue =0;
float voltage = 0;

int pinPH = A1;
float calibration_value = 21.72; //
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10], temp;

int pumpa = 6;
int pumpb = 7;
int pumpphdown = 8;
int pumpphup = 9;
int fogger = 10;
int kipas = 11;
int lamp = 12;
int aerator = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  serialtoESP.begin(9600);
  Wire.begin();
  rtc.begin();
  dht.begin();
 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
//     setting otomatis sesuai waktu program dicompile
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//     setting manual
//     January 21, 2014 at 3am you would call:
     rtc.adjust(DateTime(2024, 7, 10, 12, 50, 0));

  }
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
//  gravityTds.setPin(TdsSensorPin);
//  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
//  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
//  gravityTds.begin();  //initialization

  pinMode(pumpa, OUTPUT);
  pinMode(pumpb, OUTPUT);
  pinMode(pumpphdown, OUTPUT);
  pinMode(pumpphup, OUTPUT);
  pinMode(fogger, OUTPUT);
  pinMode(kipas, OUTPUT);
  pinMode(lamp, OUTPUT);
  pinMode (aerator, OUTPUT);
  
  lcd.init();    
  lcd.backlight();
  
  lcd.setCursor(0, 0);             //tampilan lcd awal 
  lcd.print("   MONITORING   ");
  lcd.setCursor(0, 1);
  lcd.print("    FOGPONIC    ");
  delay(2000);
  lcd.clear();  
}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();  
  Serial.print(hari);
  Serial.print(", ");
  Serial.print(jam, DEC);
  Serial.print(":");
  Serial.print(menit, DEC);
  Serial.print(":");
  Serial.print(detik, DEC);
  Serial.println("  ");
  
  for(int i=0; i<10; i++) { 
    buffer_arr[i] = analogRead(pinPH);
  delay(30);
  }
  for(int i=0; i<9; i++) {
    for(int j=i+1; j<10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  
  for(int i=2; i<8; i++)
    avgval += buffer_arr[i];
  
  float volt=(float)avgval*5.0/1024/6;
  float ph_act = -5.70 * volt + calibration_value;
  ph_act = 1.0026*ph_act - 1.0077;
  Serial.println(ph_act);

//  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
//  gravityTds.update();  //sample and calculate 
//  tdsValue = gravityTds.getTdsValue();  // then get the value
//  if (tdsValue < 8.91){
//    tdsValue = 0;
//  } else {
//    tdsValue = 1.1455*tdsValue - 9.2106;
//  }
   for (int i = 0; i < 100; i++) {
        voltage = voltage+(analogRead(TDS_SENSOR)*3.3/1024);
        delay(10);
      }
      voltage = voltage/100;
      Serial.println(voltage);
      float compVoltage=voltage/(1.0+0.02*(0));
      tdsValue = (133.42*compVoltage*compVoltage*compVoltage - 255.86*compVoltage*compVoltage + 857.39*compVoltage)*0.5; //Convert voltage value to TDS value
      if (tdsValue <97.13){
        tdsValue = 0;
      } else {
        tdsValue = 2.4989*tdsValue-241.71;
      }
  Serial.print(tdsValue,0);
  Serial.println("ppm");

  float t = dht.readTemperature();
  float h = dht.readHumidity();
//  if (isnan(h) || isnan(t)) {
//    Serial.println("Failed to read from DHT sensor!");
//    return;
//  }
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Humidity: ");
  Serial.println(h);

  lcd.setCursor(0, 0);
  lcd.print("PH:");
  lcd.print(ph_act);
 
  
  lcd.setCursor(9, 0);
  lcd.print("N:");
  lcd.print(tdsValue,0);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  lcd.print("T: ");
  lcd.print(t,1);
  lcd.print("C ");
  
  lcd.setCursor(9, 1);
  lcd.print("H: ");
  lcd.print(h,1);
  lcd.print("% ");

  digitalWrite (aerator,LOW);

  if (tdsValue < 1000) {
    digitalWrite(pumpa, LOW);
    digitalWrite(pumpb, LOW);
  }
  else {
    digitalWrite(pumpa, HIGH);
    digitalWrite(pumpb, HIGH);
  }

  if (ph_act > 6.5) {
    digitalWrite(pumpphdown, LOW);
  }
  else {
    digitalWrite(pumpphdown, HIGH);
  }
  if (ph_act < 5) {
    digitalWrite(pumpphup, LOW);
  }
  else {
    digitalWrite(pumpphup, HIGH);
  }
//
//  if (t > 25 && h < 85) {
//    digitalWrite(fogger, HIGH);
//    digitalWrite(kipas, LOW);
//  }
//  if (t < 25 && h > 95) {
//    digitalWrite(fogger, LOW);
//    digitalWrite(kipas, HIGH);
//  }
//  if (t > 25 && h > 95) {
//    digitalWrite(fogger, LOW);
//    digitalWrite(kipas, HIGH);
//  }
//  if (t < 25 && h < 85) {
//    digitalWrite(fogger, HIGH);
//    digitalWrite(kipas, LOW);
//  }
//    if (t < 25 && h > 85) {
//    digitalWrite(fogger, LOW);
//    digitalWrite(kipas, HIGH);
//  }
//    if (t < 25 && h > 95) {
//    digitalWrite(fogger, LOW);
//    digitalWrite(kipas, HIGH);
//  }
//    if (t < 25 && h > 95) {
//    digitalWrite(fogger, HIGH);
//    digitalWrite(kipas, LOW);
//  }
    if ( h > 95) {
    digitalWrite(fogger, HIGH);
    digitalWrite(kipas, LOW);
    delay(10000);
  }
      if ( h < 95) {
    digitalWrite(fogger, LOW);
    digitalWrite(kipas, HIGH);
    delay(10000);
  }

  if (jam >= 5 && jam <= 18) {
    digitalWrite(lamp, LOW);
  }
  if (jam < 5) {
    digitalWrite(lamp, HIGH);
  }
  if (jam > 18) {
    digitalWrite(lamp, HIGH);
  }

  int datapumpa = digitalRead(pumpa);
  int datapumpb = digitalRead(pumpb);
  int datapumpphdown = digitalRead(pumpphdown);
  int datapumpphup = digitalRead(pumpphup);
  int datafogger = digitalRead(fogger);
  int datakipas = digitalRead(kipas);
  int datalamp = digitalRead(lamp);
  int dataaerator = digitalRead(aerator);

  Serial.print(datapumpa);
  
  // -- Kirim data ke ESP --
  serialtoESP.print(ph_act);
  serialtoESP.print("A");
  serialtoESP.print(tdsValue);
  serialtoESP.print("B");
  serialtoESP.print(t);
  serialtoESP.print("C");   
  serialtoESP.print(h);
  serialtoESP.print("D");
  serialtoESP.print(datapumpa);
  serialtoESP.print("E");
  serialtoESP.print(datapumpb);
  serialtoESP.print("F");
  serialtoESP.print(datapumpphdown);
  serialtoESP.print("G");
  serialtoESP.print(datapumpphup);
  serialtoESP.print("H");
  serialtoESP.print(datafogger);
  serialtoESP.print("I");
  serialtoESP.print(datakipas);
  serialtoESP.print("J");
  serialtoESP.print(datalamp);
  serialtoESP.print("K");
  serialtoESP.print(dataaerator);
  serialtoESP.print("L");
  serialtoESP.print("\n");
  
  delay(1000);
}
