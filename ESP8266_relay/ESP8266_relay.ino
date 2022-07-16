#include <ESP8266WiFi.h>

int relayPin = 0;       // 0 is pin "D3" on the ESP8266
int wifiSearching = 15; // pin "D8" on the ESP8266
int wifiConnected = 16; // pin "D0" on the ESP8266

// WiFi Setup
const char *ssid = "<WIFI_SSID>";
const char *password = "<WIFI_PASSWORD>";

// Static IP address configuration
IPAddress staticIP(192, 168, 1, 201);
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

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  wifiServer.begin();

  // Sets the functions of each pin
  pinMode(relayPin, OUTPUT);
  pinMode(wifiSearching, OUTPUT); // declare wifi searching LED as output
  pinMode(wifiConnected, OUTPUT); // declare wifi connected LED as output

  // Sets initial state(s) of pin(s)
  digitalWrite(relayPin, LOW);
  digitalWrite(wifiSearching, LOW);
  digitalWrite(wifiConnected, LOW);

  delay(2000);
}

void loop()
{

  WiFiClient client = wifiServer.available();

  if (client)
  {
    wifiConnect();
    while (client.connected())
    {
      // Captures raw data from the client connection
      // and sends it to processing
      while (client.available() > 0)
      {
        char c = client.read();
        processReceivedValue(c);
        Serial.write(c);
      }
      delay(10);
    }

    client.stop();
    Serial.println("Client disconnected");
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

// Processes client commands and executes actions on the board
void processReceivedValue(char command)
{
  if (command == '1')
  {
    digitalWrite(relayPin, HIGH);
  }
  else if (command == '0')
  {
    digitalWrite(relayPin, LOW);
  }
  return;
}
