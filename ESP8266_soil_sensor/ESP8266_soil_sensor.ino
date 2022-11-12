#include <ESP8266WiFi.h>

int soilSensorPin = A0; // pin "A0" on the ESP8266
int wifiSearching = 15; // pin "D8" on the ESP8266
int wifiConnected = 16; // pin "D0" on the ESP8266

bool initialDataSent = false;
int sensorValue = 0;
String soilStatus;
String currentStatus;

// WiFi Setup
const char *ssid = "<WIFI_SSID>";
const char *password = "<WIFI_PASSWORD>";

// Static IP address configuration
IPAddress staticIP(192, 168, 1, 220);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
WiFiServer wifiServer(80);

void setup()
{

  // Initialization
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, subnet, gateway, dns);
  WiFi.begin(ssid, password);

  // Attempts to connect to WiFi before main loop
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Begins WiFi Network
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  wifiServer.begin();

  // Sets the functions of each pin
  pinMode(soilSensorPin, INPUT);  // declare sensor as input
  pinMode(wifiSearching, OUTPUT); // declare wifi searching LED as output
  pinMode(wifiConnected, OUTPUT); // declare wifi connected LED as output

  // Assigning initial state
  digitalWrite(wifiSearching, LOW);
  digitalWrite(wifiConnected, LOW);

  delay(2000);
}

void loop()
{

  WiFiClient client = wifiServer.available();

  // Triggers upon a successful socket connection
  if (client)
  {
    wifiConnect();

    while (client.connected())
    {
      delay(1000);
      currentStatus = getSoilStatus(analogRead(soilSensorPin));
      if (currentStatus != soilStatus)
      {
        Serial.println("Soil Changed Status.");
        soilStatus = currentStatus;
        client.print(soilStatus);
      }
      // Sends the initial soil data upon client connection
      if (initialDataSent == false)
      {
        client.print(soilStatus);
        initialDataSent = true;
      }
      // Soil Sample interval
      delay(2000);
    }
    client.stop();
    Serial.println("Client Disconnected");
    initialDataSent = false;
    wifiDisconnect();
  }
  else
  {
    Serial.println("Waiting for connection...");
    toggleWifiLight();
    delay(1000);
  }
}

void toggleWifiLight()
{
  long wifiState = digitalRead(wifiSearching);
  if (wifiState == LOW)
  {
    digitalWrite(wifiSearching, HIGH);
  }
  else if (wifiState == HIGH)
  {
    digitalWrite(wifiSearching, LOW);
  }
}

void wifiConnect()
{
  digitalWrite(wifiSearching, LOW);
  digitalWrite(wifiConnected, HIGH);
}

void wifiDisconnect()
{
  digitalWrite(wifiConnected, LOW);
}

String getSoilStatus(int value)
{
  if (value > 600)
  {
    return "dry";
  }
  else if (value > 300)
  {
    return "damp";
  }
  else if (value > 150)
  {
    return "moist";
  }
  else
  {
    return "wet";
  }
}
