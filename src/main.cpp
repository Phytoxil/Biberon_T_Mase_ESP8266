#include <PubSubClient.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <info.h>
#include <settings.h>


WiFiClient wifiClient;
PubSubClient MQTTclient(wifiClient);
unsigned long lastMSG = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
void MQTTconnect();

void control_motor(int Speed, bool direction){
  digitalWrite(PIN_Motor_DIR, direction); // direction low ==> backward or high ==> forward
  analogWrite(PIN_Motor_PWR, Speed); // Speed from 0 to SPEED = 255
}
void decalage(bool direction, float temps_seconds){
  for (int i = 1000; i > 0; i--){ // ce for fait en sorte que la tige s'enlève de l'interrupteur, pour qu'il retrouve son état HIGH
    control_motor(SPEED, direction);
    delay(temps_seconds);
  }
}

void MQTTsubscrib(){

    MQTTclient.subscribe(topic_right_state_set.c_str());
    //MQTTclient.subscribe("device/camera");
    MQTTclient.publish("souris_city/01/local/$connected", "bottle_right",true);
    String jsonString = "{"
                    "\"type\": \"bottles\","
                    "\"location\": \"T_MAZE\","
                    "\"properties\": ["
                    "{"
                    "\"name\": \"state\","
                    "\"datatype\": \"str\","
                    "\"settable\": true,"
                    "\"retain\": false,"
                    "\"static\": false,"
                    "\"static_value\": null"
                    "}"
                    "]"
                    "}";
    MQTTclient.publish(topic_right_meta.c_str(),jsonString.c_str(),true);/*donne le detail des propriétes. ()*/
}
void MQTTconnect(){
  while (!MQTTclient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    Serial.println(clientId.c_str());
    // test connexion
    if (MQTTclient.connect(clientId.c_str())){
      Serial.println("Connected to MQTT broker");
      Serial.println(server.toString());
      digitalWrite(PIN_LED_MQTT, HIGH);
      MQTTsubscrib();
    }else{
      Serial.print("ECHEC, rc=");
      digitalWrite(PIN_LED_MQTT, LOW);
      Serial.print(MQTTclient.state());
      Serial.println("nouvelle tentative dans 5 secondes");
      delay(5000);
  }
  }
}

void calibrateMotor() {
  // Déplacer le moteur jusqu'à ce qu'il atteigne l'interrupteur de position initiale
  while (digitalRead(PIN_Switch_positionInitiale) != LOW) {
  control_motor(SPEED, HIGH); //dispo
  }
  control_motor(0, NULL);
  decalage(LOW, 1.5);
  control_motor(0, NULL);
}

void callback(char* topic, byte *payload, unsigned int length);



void setup() {

  Serial.begin(115200);

  pinMode(PIN_Motor_PWR, OUTPUT);
  pinMode(PIN_Motor_DIR, OUTPUT);
  pinMode(PIN_Switch_Stop, INPUT);
  pinMode(PIN_Switch_positionInitiale, INPUT);
  pinMode(PIN_LED_MQTT, OUTPUT);

  // Connexion au réseau WiFi
  WiFi.begin(SSID, PASSWORD);
  //WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau WiFi...");
  }
  Serial.print("Connecté au réseau WiFi");
  Serial.println(SSID);
  Serial.print("IP Address");
  Serial.println(WiFi.localIP());
  
  MQTTclient.setServer(server, 1883);
  MQTTclient.setCallback(callback);
  MQTTclient.setKeepAlive(5);

  calibrateMotor(); //calibrer le moteur au demarrage
}

void loop() {
   if (!MQTTclient.connected()) {
    MQTTconnect();
  }
  if (MQTTclient.loop()){}
  else{Serial.println("Déconnecté");
  }
  
  // Vérifier si l'interrupteur de position initiale est enfoncé
  if (digitalRead(PIN_Switch_Stop) == LOW) {
    decalage(HIGH, 1.5);
    control_motor(0, NULL);
    MQTTclient.publish(topic_right_state.c_str(), "0");
   
  }
  if (digitalRead(PIN_Switch_positionInitiale) == LOW) {
    decalage(LOW, 1.5);
    control_motor(0, NULL);
    MQTTclient.publish(topic_right_state.c_str(), "1");
  }
}

void callback(char* topic, byte *payload, unsigned int length) {

    Serial.print("message arrived[");
    Serial.print(topic);
    Serial.print("]");
    
    for(unsigned int i = 0; i < length; i++){
      Serial.print((char)payload[i]);
    }
  if (strcmp(topic, topic_right_state_set.c_str()) == 0) {
    if ((char)payload[0] == '0') {
      control_motor(SPEED, LOW); //pas dispo
    }
    else if ((char)payload[0] == '1') {
      control_motor(SPEED, HIGH); //dispo
  }
 }
}