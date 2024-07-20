

    // Send the POST

#include <WiFi.h>
#include <HTTPClient.h>
#include <SoftwareSerial.h>

SoftwareSerial serialtoESP(19, 18);  // RX, TX

char ph_act[10];
char tdsValue[10];
char t[10];
char h[10];
char datapumpa[10];
char datapumpb[10];
char datapumpphdown[10];
char datapumpphup[10];
char datafogger[10];
char datakipas[10];
char datalamp[10];
char dataaerator[10];

char c;
String dataIn;
int8_t indexA, indexB, indexC, indexD, indexE, indexF, indexG;
int8_t indexH, indexI, indexJ, indexK, indexL;
String apiKeyValue = "12345678912";
int ledwifi = 2;

//String a = "99";
//String b = "20";
//String m = "800";
//String d = "0";
//String e = "1";
//String f = "1";
//String g = "0";
//String l = "1";
//String i = "1";
//String j = "1";
//String k = "1";
//String n = "1";

char ssid[] = "LabPertanianCerdas";
char pass[] = "labpc2024#";

void setup() {
  Serial.begin(9600); 
  serialtoESP.begin(9600);

  pinMode(ledwifi, OUTPUT);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    digitalWrite(ledwifi, LOW);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  digitalWrite(ledwifi, HIGH);
}

void loop() {
  // Read data from serial
  while (serialtoESP.available() > 0) {
    c = serialtoESP.read();
    if (c == '\n') {
      break;
    } else {
      dataIn += c;
    }
  }

  if (c == '\n') {
    ParseData();

    Serial.print(ph_act);
    Serial.print("  ");
    Serial.print(tdsValue);
    Serial.print("  ");
    Serial.print(t);
    Serial.print("  ");
    Serial.print(h);
    Serial.print("  ");
    Serial.print(datapumpa);
    Serial.print("  ");
    Serial.print(datapumpb);
    Serial.print("  ");
    Serial.print(datapumpphdown);
    Serial.print("  ");
    Serial.print(datapumpphup);
    Serial.print("  ");
    Serial.print(datafogger);
    Serial.print("  ");
    Serial.print(datalamp);
    Serial.print("  ");
    Serial.print(dataaerator);
    
    c = 0;
    dataIn = "";
  }

  sendDataToServer();
  delay(1000); // Send data every 10 seconds
}

void ParseData() {
  indexA = dataIn.indexOf("A");
  indexB = dataIn.indexOf("B");
  indexC = dataIn.indexOf("C");
  indexD = dataIn.indexOf("D");
  indexE = dataIn.indexOf("E");
  indexF = dataIn.indexOf("F");
  indexG = dataIn.indexOf("G");
  indexH = dataIn.indexOf("H");
  indexI = dataIn.indexOf("I");
  indexJ = dataIn.indexOf("J");
  indexK = dataIn.indexOf("K");
  indexL = dataIn.indexOf("L");
  
  strcpy(ph_act, dataIn.substring(0, indexA).c_str());
  strcpy(tdsValue, dataIn.substring(indexA + 1, indexB).c_str());
  strcpy(t, dataIn.substring(indexB + 1, indexC).c_str());
  strcpy(h, dataIn.substring(indexC + 1, indexD).c_str());
  strcpy(datapumpa, dataIn.substring(indexD + 1, indexE).c_str());
  strcpy(datapumpb, dataIn.substring(indexE + 1, indexF).c_str());
  strcpy(datapumpphdown, dataIn.substring(indexF + 1, indexG).c_str());
  strcpy(datapumpphup, dataIn.substring(indexG + 1, indexH).c_str());
  strcpy(datafogger, dataIn.substring(indexH + 1, indexI).c_str());
  strcpy(datakipas, dataIn.substring(indexI + 1, indexJ).c_str());
  strcpy(datalamp, dataIn.substring(indexJ + 1, indexK).c_str());
  strcpy(dataaerator, dataIn.substring(indexK + 1, indexL).c_str());
}

void sendDataToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://sheisfonic.pertaniancerdas.com/data.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Building POST data
    String postData = "api_key=" + String(apiKeyValue);
    postData += "&ph_act=" + String(ph_act);
    postData += "&tdsValue=" + String(tdsValue);
    postData += "&t=" + String(t);
    postData += "&h=" + String(h);
    postData += "&datapumpa=" + String(datapumpa);
    postData += "&datapumpb=" + String(datapumpb);
    postData += "&datapumpphdown=" + String(datapumpphdown);
    postData += "&datapumpphup=" + String(datapumpphup);
    postData += "&datafogger=" + String(datafogger);
    postData += "&datakipas=" + String(datakipas);
    postData += "&datalamp=" + String(datalamp);
    postData += "&dataaerator=" + String(dataaerator);
    
      Serial.println("postData: ");
      Serial.println(postData);
  
    // Sending POST data to the server
    int httpCode = http.POST(postData);
  
    // Checking HTTP response
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    } else {
      Serial.println("Error on HTTP request");
    }
  
    http.end();
  }
}
