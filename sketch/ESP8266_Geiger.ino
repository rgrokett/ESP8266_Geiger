/*
 *  Geiger Counter
 *  Collects counts and sends to ThingSpeak.com for graphs
 *
 *  Requires HTTPS SSL
 *  To get SHA1 Fingerprint:
 *  $ openssl s_client -servername api.thingspeak.com -connect api.thingspeak.com:443 | openssl x509 -fingerprint -noout
 *
 *  version 1.2 2016-01-11 R.Grokett
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid     = "2WIRE683";  // Your WiFi SSID
const char* password = "8664609433"; // Your WiFi Password

int freq = 1;     // Collect and send data updates every X minutes
const char* host = "api.thingspeak.com";
const int port = 443;
const char* api_key = "BFXZG7F92XMNVIP4"; // Your API KEY from www.thingspeak.com 
const char* SHA1Fingerprint="78 60 18 44 81 35 BF DF 77 84 D4 0A 22 0D 9B 4E 6C DC 57 2C";  // See SHA1 comment above
bool verifyCert = true; // Select false if you do NOT want SSL certificate validation


// Conversion factor - CPM to uSV/h
//#define CONV_FACTOR 0.0057     // SBM-20 tube Cs137
#define CONV_FACTOR 0.00812     // LND712 tube Cs137 

int LED = 0;    // GPIO 0 (built-in LED)

int gpio_input = 5;
long count = 0;
long countPerMinute = 0;
long timePrevious = 0;
long timePreviousMeassure = 0;
//long time = 0;
long countPrevious = 0;
float radiationValue = 0.0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare Geiger input pin
  pinMode(gpio_input, INPUT);
  digitalWrite(gpio_input, HIGH);

  // prepare onboard LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);  
  
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Blink onboard LED to signify its connected
  blink();
  blink();
  blink();
  blink(); 

  attachInterrupt(digitalPinToInterrupt(gpio_input),countPulse,FALLING);
  
}



void loop() {
  if (millis()-timePreviousMeassure > (freq * 60L * 1000L)){   // milliseconds 
    detachInterrupt(digitalPinToInterrupt(gpio_input));
    countPerMinute = count/freq;    // TODO Convert to rolling average   
    radiationValue = countPerMinute * CONV_FACTOR;
    timePreviousMeassure = millis();
    Serial.print("cpm = "); 
    Serial.print(countPerMinute,DEC);
    Serial.print(" - ");
    Serial.print("uSv/h = ");
    Serial.println(radiationValue,4);      

    Serial.print("connecting to ");
    Serial.println(host);
  
    // Use WiFiClient class to create TCP connections
    WiFiClientSecure client;
    if (!client.connect(host, port)) {
      Serial.println("connection failed");
      return;
    }

	if (verifyCert) {
	  if (!client.verify(SHA1Fingerprint, host)) {
        Serial.println("certificate doesn't match. will not send message.");
        return;
	  } 
	}
  
    // We now create a URI for the request
    String url = "/update?api_key="+String(api_key)+"&field1="+String(countPerMinute)+"&field2="+String(radiationValue);
    Serial.print("Requesting URL: ");
    Serial.println(url);
  
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    delay(10);
  
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  
    Serial.println();
    Serial.println("closing connection");
    blink(); 
    attachInterrupt(digitalPinToInterrupt(gpio_input),countPulse,FALLING);
	count = 0;
  }
  delay(50);
}

void countPulse(){
  // uncomment lines below if geiger pulses are not clean
  //detachInterrupt(digitalPinToInterrupt(gpio_input));
  count++;
  //while(digitalRead(gpio_input)==0){
  //}
  //attachInterrupt(digitalPinToInterrupt(gpio_input),countPulse,FALLING);
}

void blink() {
  digitalWrite(LED, LOW);
  delay(100); 
  digitalWrite(LED, HIGH);
  delay(100);
}


