#include <SPI.h>
#include "DHT.h"
#include <ArduinoMqttClient.h>
#include <Ethernet.h>
#define DHTPIN 2       // which pin we're connected to
#define DHTTYPE DHT22  // DHT 22 (AM2302)
// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;
MqttClient mqttClient(ethClient);
//DHT dht;
DHT dht(DHTPIN, DHTTYPE);
const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topic[] = "real_unique_topic_1";
const char topic2[] = "real_unique_topic_2";
const char topic3[] = "real_unique_topic_3";
//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;
int count = 0;
//sensorReadPublish data to MQTT broker
void sensorReadPublish() {
  // Read relative humidity (%)
  float h = dht.readHumidity();
  // Read temperature (Celsius)
  float t = dht.readTemperature();
  // Read heat index
  float hic = dht.computeHeatIndex(t, h);
  // Check if data reads are valid numbers
  if (isnan(h) || isnan(t)) {
    Serial.println("Data error from DHT11 sensor!");
    return;
  }
  // publish temperature to topic2
  mqttClient.beginMessage(topic2);
  mqttClient.print(t);
  mqttClient.endMessage();
  // publish humidity to topic
  mqttClient.beginMessage(topic);
  mqttClient.print(h);
  mqttClient.endMessage();
  // publish heatindex to topic3
  mqttClient.beginMessage(topic3);
  mqttClient.print(hic);
  mqttClient.endMessage();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" oC ");
  Serial.print("\t");
  Serial.print("Heat Index: ");
  Serial.print(hic);
  Serial.print("\n");
}
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
  }
  // connect to network:
  Ethernet.begin(mac);
  Serial.println();
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1)
      ;
  }
  Serial.println("You're connected to the MQTT broker!");
  dht.begin();
  Serial.println();
}
void loop() {
  // call poll() regularly to allow the library to send MQTT keep
  // alive which avoids being disconnected by the broker
  mqttClient.poll();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
    sensorReadPublish();
    Serial.println();
  }
}