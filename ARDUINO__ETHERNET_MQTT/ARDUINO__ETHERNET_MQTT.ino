/**
 * WIRING
 * ARDUINO    ETHERNET ENC28J60
 *  13  --------- SCK
 *  12  --------- SO
 *  11  --------- SI
 *  10  --------- CS
 *  VCC --------- VCC
 *  GND --------- GND
 * */


#include <PubSubClient.h>//Included on Community Libraries Go to Sketch->Include Library -> Manage Libraries -> Search by PubSubClient and install
#include <UIPEthernet.h>//https://github.com/UIPEthernet/UIPEthernet
#define LIGHT 4
#define MQTT_AUTH true
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"
const String HOSTNAME  = "nome_do_device_deve_ser_unico";
const char* MQTT_SERVER = "ip_ou_dns_do_servidor_mqtt";

const String MQTT_LIGHT_TOPIC = "sala/luz/set"; //TOPICOS DE EXEMPLO PARA ALTERAÇÂO DE ESTADO
const String MQTT_LIGHT_STATE_TOPIC = "sala/luz"; //TOPICOS DE EXEMPLO PARA INFORMAÇÂO DE ESTADO

EthernetClient client;
PubSubClient clientMqtt(MQTT_SERVER, 1883, client);

bool checkMqttConnection(){
  if (!client.connected()) {
    if (MQTT_AUTH ? clientMqtt.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : clientMqtt.connect(HOSTNAME.c_str())) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("MQTT CONNECTED");
      clientMqtt.subscribe(MQTT_LIGHT_TOPIC.c_str());
    }
  }
  return client.connected();
}

//Chamada de recepção de mensagem 
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
  String topicStr = String(topic);
 if(topicStr.equals(MQTT_LIGHT_TOPIC)){
  if(payloadStr.equals("ON")){
      turnOn();
    }else if(payloadStr.equals("OFF")) {
      turnOff();
    }
  } 
} 
void turnOn(){
  Serial.println("ON");
  digitalWrite(LIGHT,HIGH);
  clientMqtt.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"ON");  
}

void turnOff(){
  digitalWrite(LIGHT,LOW);  
  Serial.println("OFF");
  clientMqtt.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"OFF");
}

void setup() {
  Serial.begin(9600);
  
  pinMode(LIGHT, OUTPUT);
  
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  Ethernet.begin(mac); //Configure IP address via DHCP
  Serial.println(Ethernet.localIP());
  Serial.println(Ethernet.subnetMask());
  Serial.println(Ethernet.gatewayIP());
  Serial.println(Ethernet.dnsServerIP());
  clientMqtt.setCallback(callback);

}

void loop() {
  if (checkMqttConnection()){
      clientMqtt.loop();
    }
}

