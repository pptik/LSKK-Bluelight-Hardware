/*
    Develop by      : Fahmi Nurfadilah 
    Email           : fahmi.nurfadilah1412@gmail.com
    Updated by      : Vitradisa Pratama
    Email           : vitradisa@pptik.itb.ac.id
    Project         : LSKK Bluelight
    Version         : 2.1
*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Update these with values suitable for your network.

const char* ssid = "RuangIOT";
const char* password = "lskkiot2019";
const char* mqtt_server = "192.168.0.5";
const char* mqtt_user = "/Homeauto:homeauto";
const char* mqtt_pass = "homeauto12345!";
const char* CL = "bl-01";
const char* mqtt_pub_topic = "Sensor";

char msg[100];

WiFiClient espClient;
PubSubClient client(espClient);

byte mac[6];
String MACAddress;

const int smokePin = D0;
const int LED = D2;
const int buzzer = D3;

int smokeState = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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
}

String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED, HIGH);   // Turn the LED on (Note that HIGH is the voltage level
  } else {
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage LOW
  }
}

void reconnect() {
  // Loop until we're reconnected
  printMACAddress();
  const char* CL;
  CL = MACAddress.c_str();
  Serial.println(CL);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CL, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(smokePin, INPUT);
  digitalWrite(buzzer, LOW);
  Serial.begin(115200);
  setup_wifi();
  printMACAddress();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {
  String pubmsg = "";
  String input_guid = "812f25e0-7a52-4388-b027-67ee09150b48";
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);
  smokeState = digitalRead(smokePin);
  if (smokeState == LOW) {
    digitalWrite(buzzer, HIGH);
    //digitalWrite(buzzer, LOW);
    Serial.println(smokeState);
    //delay(5);
    
  String jenis_iot = "Input-Sensor-Asap";
  String mac = MACAddress;
  int value = (smokeState);
  Serial.print("Publish message: ");
  pubmsg = input_guid + "#" + value;
  Serial.println(pubmsg);
  client.publish(mqtt_pub_topic, pubmsg.c_str());
  delay(5000);
  } else if (smokeState == HIGH)  {
    digitalWrite(buzzer, LOW);
    //digitalWrite(buzzer, HIGH);
    Serial.println("Safe Zone");
    Serial.println(MACAddress);
  }
}
