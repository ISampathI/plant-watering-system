#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_TEMPLATE_ID "TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "DEVICE_NAME"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN"

#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;

const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

char ssid[] = "SSID";
char pass[] = "PASSWORD";

const int s0pin = D5;
const int s1pin = D6;
const int s2pin = D7;

const int motorpin = D2;

BlynkTimer timer;

int smvalue = 0;
int tempvalue = 0;
int ldrvalue = 0;

int motorspeed = 255;

BLYNK_WRITE(V0)
{
  int value = param.asInt();
  if (value){
    analogWrite(motorpin, motorspeed);  
    updateDate();
    
  }else{
    analogWrite(motorpin, 0);;
  }
  Serial.println(value);
}

BLYNK_WRITE(V2)
{
  motorspeed = param.asInt();
}
void updateDate(){
    timeClient.update();
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime); 
    String currentDate = String(ptm->tm_year+1900) + "-" + String(ptm->tm_mon+1) + "-" + String(ptm->tm_mday)+" " +timeClient.getFormattedTime();
    Blynk.virtualWrite(V3, currentDate);
}

void setMoistureVal()
{
  selSen1();
  delay(200);
  smvalue = analogRead(A0);

  selSen2();
  delay(200);
  tempvalue = analogRead(A0);

  selSen3();
  delay(200);
  ldrvalue = analogRead(A0);
  
  if (smvalue > 750){
    analogWrite(D2, motorspeed);   
    Blynk.virtualWrite(V0, 1);       
    updateDate();
    
  }else{
    analogWrite(D2, 0);
    Blynk.virtualWrite(V0, 0);
  }
  Blynk.virtualWrite(V1, smvalue);
  Blynk.virtualWrite(V4, ldrvalue);
  Blynk.virtualWrite(V5, tempvalue);
}
void selSen1(){
  digitalWrite(s0pin, LOW);
  digitalWrite(s1pin, LOW);
  digitalWrite(s2pin, LOW);
}
void selSen2(){
  digitalWrite(s0pin, LOW);
  digitalWrite(s1pin, LOW);
  digitalWrite(s2pin, HIGH);
}
void selSen3(){
  digitalWrite(s0pin, LOW);
  digitalWrite(s1pin, HIGH);
  digitalWrite(s2pin, LOW);
}
void setup()
{
  Serial.begin(115200);
  timeClient.begin();
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(s0pin, OUTPUT);
  pinMode(s1pin, OUTPUT);
  pinMode(s2pin, OUTPUT);

  digitalWrite(s0pin, LOW);
  digitalWrite(s1pin, LOW);
  digitalWrite(s2pin, LOW);
  
  //Blynk.begin(auth, ssid, pass);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  analogWriteFreq(500);
  analogWriteRange(255);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,101), 9443);
  timer.setInterval(5000L, setMoistureVal);
}

void loop()
{
  Blynk.run();
  timer.run();
}
