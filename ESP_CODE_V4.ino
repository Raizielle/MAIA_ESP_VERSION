//////////////////////////////////// MAIA - MOBILE AWARE INTERMODAL ASSISTANT /////////////////////////////////
/////////////////////////////// CENTRO DE PESQUISA, DESENVOLVIMENTO E INOVAÇÃO DELL ///////////////////////////

///////////////////////////////////////////////// BIBLIOTECAS /////////////////////////////////////////////////

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

///////////////////////////////////////////////// DEFINIÇÕES /////////////////////////////////////////////////

#define ssid         "DELL_WIFI"
#define password     "B1c4@(wifi)"
#define mqttServer   "192.168.1.251"
#define mqttPort     1883
#define mqttUser     "mqtt-test"
#define mqttPassword "mqtt-test"

//////////////////////////////////////////////// VARIÁVEIS ////////////////////////////////////////////////

String requisicao = "";
String arduinoresponse = "";
char aux = ' '; 
boolean flag;
boolean msg;

///////////////////////////////////////////////// OBJETOS /////////////////////////////////////////////////

WiFiClient espClient;
PubSubClient client(espClient);                    
SoftwareSerial arduino(5, 4);
                    // RX, TX
                    
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  arduino.begin(9600);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi ...");
  }
  Serial.print("Conectado com sucesso à rede ");
  Serial.println(ssid);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Conectando ao broker ...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("Conexão bem sucedida!");  
    } else {
      Serial.print("Falha na conexão. Estado: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  flag = false;
  msg = false;
  client.subscribe("maia/chamadas");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void callback(char* topic, byte* payload, unsigned int length) {
  if(flag==false){
    Serial.println("Requisição chegada e entrando em rocesso de atendimento!");
    requisicao = "";
    Serial.print("Mensagem chegada no tópico: ");
    Serial.println(topic);
    Serial.print("Mensagem: ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      requisicao+=(char)payload[i];
    }
    Serial.println();
    Serial.println("--------------------------------------------------------");
    Serial.print("String Obtida: ");
    Serial.print("'"); 
    Serial.print(requisicao);
    Serial.println("'");
    flag = true;
  }
  else{
    Serial.println("Requisição chegada e ignorada");
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  if(flag==true){
    if(msg == false){
      Serial.println("Enviando mensagem ao arduino ...");
      Serial.println(requisicao);
      arduino.println(requisicao);   
    }
    msg = true;
    if(arduino.available()){
      Serial.println("Recebendo mensagem do Arduino ...");
      while(arduino.available()){
        aux = arduino.read();
        arduinoresponse+=aux;
      }
      Serial.print("Resposta do arduíno: ");
      Serial.println(arduinoresponse);
      client.publish("maia/respostas", (char*)arduinoresponse.c_str());
      Serial.println("Mensagem publicada...");
      flag = false;
      msg = false;
      arduinoresponse = "";
    }
  }
  client.loop();
}   

////////////////////////////////////////////////////////////////////////////////////////////////////////////
