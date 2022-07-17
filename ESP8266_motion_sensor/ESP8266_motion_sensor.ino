#include <ESP8266WiFi.h>

int statusPin = 12;     // pin "D6" on the ESP8266
int sensorPin = 13;     // pin "D7" on the ESP8266
int wifiSearching = 15; // pin "D8" on the ESP8266
int wifiConnected = 16; // pin "D0" on the ESP8266

long state;
long prevstate;
int result;

// WiFi Setup
const char *ssid = "<WIFI_SSID>";
const char *password = "<WIFI_PASSWORD>";

// Static IP address configuration
IPAddress staticIP(192, 168, 1, 121);
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
  pinMode(sensorPin, INPUT);      // declare sensor as input
  pinMode(statusPin, OUTPUT);     // declare sensor LED as output
  pinMode(wifiSearching, OUTPUT); // declare wifi searching LED as output
  pinMode(wifiConnected, OUTPUT); // declare wifi connected LED as output

  // Assigning initial state
  prevstate = LOW;
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
      state = digitalRead(sensorPin);
      toggleSensorLight(state);
      result = checkForStateChange(state, prevstate);
      if (result == 1)
      {
        client.print("on");
        delay(1000);
      }
      else if (result == 0)
      {
        client.print("off");
        delay(1000);
      }
      prevstate = state;
      delay(10);
    }
    client.stop();
    Serial.println("Client Disconnected");
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
  digitalWrite(statusPin, LOW);
}

void toggleSensorLight(long state)
{
  if (state == HIGH)
  {
    digitalWrite(statusPin, HIGH);
  }
  else
  {
    digitalWrite(statusPin, LOW);
  }
}

int checkForStateChange(long state, long prevstate)
{
  if (prevstate == LOW && state == HIGH)
  {
    return 1;
  }
  else if (prevstate == HIGH && state == LOW)
  {
    return 0;
  }
  else
  {
    return 99;
  }
}
