#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

#define PIN_Motor_PWR 5
#define PIN_Motor_DIR 14
#define PIN_Switch_positionInitiale 12 //anciennement position initiale
#define PIN_Switch_Stop 4 //anciennement Switch_Stop
#define PIN_LED_MQTT 0

const int SPEED = 255;
const int STEP_DEPART = 0;
const int STEP_ARRIVEE = 100;
                       

//info connexion
const char * SSID = "DESKTOP-steve-test";
const char * PASSWORD = "f5O:1480";


String clientId = "ESP8266_Yanis_right"; //bien veiller à mettre le bon idenfiant (left ou right)
//String Experience ="ESP8266/";
//String Type_device = "motor/";
//String Topic_set = Experience + Type_device + clientId + "/set";
//String Topic_connected = "devices/connected/"+clientId;
//String Topic_list = "devices/list/"+clientId;
//String Topic_base = Experience + Type_device + clientId +"/";

String id_mqtt = clientId;
String pass_mqtt = "yanis";
String Hostname_wifi = clientId;

//IP address
IPAddress ip(192,168,137,51);
IPAddress server(192,168,24,120);
IPAddress gateway(192,168,137,1);
