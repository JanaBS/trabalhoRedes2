#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <Wire.h>
#include "DHT.h"
#include <stdio.h>
#include <string.h>

DHT dht(22, DHT11);
WiFiClient espClient;
PubSubClient client(espClient);

// servidor mqtt online
const char* mqtt_server = "mqtt.thingsboard.cloud";

int luz;
float temperatura;
float umidade;
char str[] = "}";

void setup_wifi() {
  delay(10);
  WiFi.begin("Galaxy A329A3D", "laol2360");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar...");
    if (client.connect("ESP8266Client", "2IoM5nnHPxsoV2ugwI6l", "2IoM5nnHPxsoV2ugwI6l")) {
      Serial.println("conectado");
    } else {
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(2000);

  luz = analogRead(A0);
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();

  // Se ocorreu alguma falha durante a leitura
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Falha na leitura do Sensor DHT!");
  }
  else {
    //monta a mensagem da temperatura
    char mensagemT[100] = "{'temperature':";
    char temp[50];
    sprintf(temp, "%.2f", temperatura);  
    strcat(mensagemT, temp);
    strcat(mensagemT, str);

    //monta a mensagem da umidade
    char mensagemU[100] = "{'humidity':";
    char umid[50];
    sprintf(umid, "%.2f", umidade);
    strcat(mensagemU, umid);
    strcat(mensagemU, str);

    //monta a mensagem da luz
    char mensagemL[100] = "{'light':";
    char light[50];
    sprintf(light, "%d", luz);
    strcat(mensagemL, light);
    strcat(mensagemL, str);
 
    client.publish("v1/devices/me/telemetry", mensagemT);
    client.publish("v1/devices/me/telemetry", mensagemU);
    client.publish("v1/devices/me/telemetry", mensagemL);
  }
}