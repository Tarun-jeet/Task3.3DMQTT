#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

// Define pins for Ultrasonic Sensor and LED
#define TRIG_PIN 2
#define ECHO_PIN 3
#define LED_PIN 4
int status = WL_IDLE_STATUS;

// Wi-Fi Credentials
char ssid[] = SECRET_SSID;        
char pass[] = SECRET_PASS;

// MQTT Broker Details
const char* mqtt_server = "p586ca11.ala.dedicated.aws.emqxcloud.com";
const int mqtt_port = 1883;
const char* mqtt_user = "tarunjeet";         // MQTT Username
const char* mqtt_password = "tarun13";        // MQTT Password
const char* mqtt_clientId = "Task3.3D";       // MQTT Client ID
const char* topic_publish = "SIT210/wave";
const char* topic_subscribe = "SIT210/wave";

// Creating Wi-Fi and MQTT client objects
WiFiClient client;
PubSubClient mqtt_client(client);

unsigned long currentMillis, previousMillis, dataSendingInterval = 1000; // 1 second interval

void setup() {
  
  Serial.begin(115200);

  // Initializing LED and Ultrasonic sensor pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  while (!Serial) 
  {
    ; // wait for serial port to connect. 
  }
  // Connect to Wi-Fi
  setup_wifi();
  delay(10000);
  Serial.println("WIFI CONNECTED!! ");
  
  // Set MQTT server and callback function
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);

  // Connect to MQTT broker
  mqtt_connect();
}

void loop() {
  //if not connected then again trying to connect
  if (!mqtt_client.connected()) {
    mqtt_connect();
  }
  
  mqtt_client.loop();

  // Checking for Ultrasonic sensor wave detection
  long distance = measureDistance();
  if (distance < 20) {  // Adjusting threshold
    Serial.println("Wave detected!");

    // Published my name to the topic
    String message = "Tarunjeet";
    mqtt_client.publish(topic_publish, message.c_str());

    delay(1000);  // Add delay to avoid multiple triggers
  }
}

// Function to measure distance using Ultrasonic Sensor
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;  // Convert duration to distance in cm
  return distance;
}

// Connect to Wi-Fi
void setup_wifi()
{
  //checking for the wifi module if connected or not with conditional statement
  if (WiFi.status() == WL_NO_MODULE) 
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue if not connected
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  //checking if the module has latest wifi firmware
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
  {
    Serial.println("Please upgrade the firmware");
  }

  //if not connected to wifi then trying unitl connected
  while (status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    
  }
}

// Connect to MQTT broker
void mqtt_connect() {
  //connecting the mqtt repeatedly till not connected
  while (!mqtt_client.connected()) {
    Serial.print("Connecting to MQTT...");
    //if connected then providing my mqtt details
    if (mqtt_client.connect(mqtt_clientId, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      mqtt_client.subscribe(topic_subscribe);  // Subscribe to the topic
    } 
    //error if not connected successfully
    else {
      Serial.print("Failed to connect, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);  // Wait 5 seconds before retrying
    }
  }
}

// Callback function when a message is received
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  // Print message
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message: " + message);

  // Blink LED 3 times when a message is received
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}
