#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Alfyan_yu";
const char* pass = "what are you say";
//isi MQTT broker memiliki username and pass
// const char* userServer = "username broker";
// const char* passServer = "password broker";
const char* broker = "test.mosquitto.org";
const char* outTopic = "bagalyus/out";
const char* inTopic = "bagalyus/in";
const int lm = A0;


WiFiClient mayClient;
PubSubClient client(mayClient);
unsigned long timeEnd = 0;
unsigned long timeStart = 0;
char msg[50];

//subscribe data dari server
void callback(char* topic, byte* data, unsigned int length) {
  Serial.print("Pesan Masuk: ");
  Serial.print(topic);
  Serial.print(": ");

  for(int i = 0; i < length; i++) {
    Serial.print((char)data[i]);
  }
  Serial.println();

  if((char)data[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED on");
  }
  else if ((char)data[0] == '0')
  {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("LED off");
  }
  
}


void reconnect(){
  while(!client.connected()){
    Serial.print("try conneting to ");
    Serial.println(broker);
    if(client.connect("katsumiYuu")) {
      Serial.print("connected to ");
      Serial.println(broker);
      client.publish(outTopic, "yo");
      client.subscribe(inTopic);
    }
    else{
      Serial.print("try connect in 5s");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(lm, INPUT);
  //seting port client.setServer(broker, "pasang id di sini");
  client.setServer(broker, 1883);
  client.setCallback(callback);

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("connected to ");
  Serial.println(ssid);
}

void loop() {
  if(!client.connected()) {
    reconnect();
  }
  client.loop();

  //up data
  timeStart = millis();
  if(timeStart - timeEnd > 1000) {
    //lm35
    float data;
    int suhuC;
    float adc;

    adc = analogRead(lm);
    data = adc / 1023 * 3.3;
    suhuC = data * 100;
    //up data
    snprintf(msg, 75, "%ld°C", suhuC);
    Serial.print("Suhu Ruangan: ");
    Serial.println(msg);
    client.publish(outTopic, msg);
    timeEnd = millis();
  }
}