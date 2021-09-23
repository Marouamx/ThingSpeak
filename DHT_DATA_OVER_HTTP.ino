/****
****  Marouamx : 13:20 09/08/2020
****  esp2866_http_usingATCommands
****/

#include "DHT.h"
#include <SoftwareSerial.h>

#define DHTTYPE DHT11
#define DHTPIN 2



DHT dht(DHTPIN,DHTTYPE);
SoftwareSerial esp(6,7); // 6 rx, 7 tx



String ssid = "****"; 
String pwd = "**********";
String WriteAPIKey = "A4M8M8YYW07MF7PI"; 
String Port = "80"; 
String Host = "api.thingspeak.com"; 
String ip = "192.168.1.52"; 


int countTrueCommand;
int countTimeCommand;

boolean found = false;  

float t,h;

void sendCommand(String command, int maxTime, char readReplay[]);
void setup_module();
String Get_temp();
String Get_Hum(); 

void setup() {
  
  Serial.begin(115200); 
  esp.begin(115200); 
  setup_module();
  dht.begin();
}

void loop() {
   
   String getData = "GET /update?api_key="+ WriteAPIKey +"&field1="+Get_Temp()+"&field2="+Get_Hum(); //construct URL
   sendCommand("AT+CIPMUX=1",5,"OK");// set multiple cnx mode
   sendCommand("AT+CIPSTART=0,\"TCP\",\""+ Host +"\","+ Port,15,"OK"); // establish TCP cnx
   sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");// length od data 
   esp.println(getData);delay(1500);countTrueCommand++;//send Data
   sendCommand("AT+CIPCLOSE=0",5,"OK"); // close TCP cnx


  delay(10000);
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp.println(command);//at+cipsend
    if(esp.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OKAY");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
 
void setup_module(){

  sendCommand("AT",5,"OK"); 
  sendCommand("AT+RST",5,"Ready");
  
  sendCommand("AT+CWMODE=1",5,"OK"); // STA mode 
  sendCommand("AT+CWDHCP=1,1",5,"OK"); // DHCP enable
  sendCommand("AT+CWQAP",5,"OK"); // Quit any AP
  //sendCommand("AT+CIPSTA_CUR=\""+ ip +"\"",5,"OK");
  sendCommand("AT+CWJAP=\""+ ssid +"\",\""+ pwd +"\"",20,"OK");// JOIN AP
    
}


String Get_Temp(){
  
  t = dht.readTemperature();
  if (isnan(t)) return "invalid_reading";
  else return String(t);

}
String Get_Hum(){
  
  h = dht.readHumidity();
  if (isnan(h)) return "invalid_reading";
  else return String(h);
}

