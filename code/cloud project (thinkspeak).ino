 #include <WiFi.h>
#include <WiFiMulti.h>
#include "DHT.h"
#define DHTPIN 2    
#define DHTTYPE DHT11 
#define MQ2pin 15
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701 
DHT dht(DHTPIN, DHTTYPE);
float sensorValue;
long duration;
float distanceCm;
float distanceInch;
 
WiFiMulti WiFiMulti;
const int trigPin = 5;
const int echoPin = 18;
const char* ssid     = ""; // Your SSID (Name of your WiFi)
const char* password = ""; //Your Wifi password

const char* host = "api.thingspeak.com";
String api_key = ""; // Your API Key provied by thingspeak

void setup(){
  Serial.begin(9600);
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   dht.begin();
   Connect_to_Wifi();
   Serial.println("*****ZOUAOUI SAIFEDDINE******");
}

void loop(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  sensorValue = analogRead(MQ2pin);
   // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C   "));
  Serial.print("  Distance (cm): ");
  Serial.print( distanceCm  );
  Serial.print("    Distance (inch): ");
  Serial.print(distanceInch);
  Serial.print("  Gaz Value: ");
  Serial.println(sensorValue);
  

 // call function to send data to Thingspeak
  Send_Data(t,h, distanceCm ,distanceInch,sensorValue);

  delay(5000);
 

}

void Connect_to_Wifi()
{

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void Send_Data(float t, float h,float  distanceCm  ,float distanceInch,float sensorValue)
{

  // map the moist to 0 and 100% for a nice overview in thingspeak.
  Serial.println("");
  Serial.println("Prepare to send data");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;


  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  else
  {
    String data_to_send = api_key;
    data_to_send += "&field1=";
    data_to_send += String(t);
    data_to_send += "&field2=";
    data_to_send += String(h);
     data_to_send += "&field3=";
    data_to_send += String(distanceCm);
    data_to_send += "&field4=";
    data_to_send += String(distanceInch);
     data_to_send += "&field5=";
    data_to_send += String(sensorValue);
    // data_to_send += "&field3=";
    // data_to_send += String(pressure);
    data_to_send += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data_to_send.length());
    client.print("\n\n");
    client.print(data_to_send);

    delay(1000);
  }

  client.stop();

}
