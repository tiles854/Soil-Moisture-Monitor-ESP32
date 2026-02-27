#define BLYNK_TEMPLATE_ID "TMPL3TC2hUi6_"
#define BLYNK_TEMPLATE_NAME "soil Moisture Monitor"
#define BLYNK_AUTH_TOKEN "pIKbJGfeOC8qNtj9jj1MaKYNUN68p7QX"

#include <WiFi.h>
#include <WebServer.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "NavGurukul3";
char pass[] = "Nav_gurukul_25.Org";

const char* ap_ssid = "SoilMonitor";
const char* ap_pass = "12345678";

#define SOIL_PIN 34
#define LED_PIN 2

WebServer server(80);
BlynkTimer timer;

int dryValue = 30;
int wetValue = 5;

int moisturePercent = 0;


String webpage()
{
  String page = "<html><head>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<meta http-equiv='refresh' content='2'>";
  page += "<style>";
  page += "body{font-family:Arial;text-align:center;background:#e8f5e9;}";
  page += "h1{color:#2e7d32;}";
  page += ".box{font-size:40px;background:white;padding:20px;border-radius:10px;display:inline-block;}";
  page += "</style></head><body>";
  page += "<h1>🌱 Soil Moisture</h1>";
  page += "<div class='box'>";
  page += String(moisturePercent) + " %";
  page += "</div></body></html>";
  return page;
}

void handleRoot()
{
  server.send(200, "text/html", webpage());
}

void sendSoilData()
{
  int soilValue = analogRead(SOIL_PIN);

  moisturePercent = map(soilValue, dryValue, wetValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Moisture %: ");
  Serial.println(moisturePercent);

  if (Blynk.connected())
  {
    Blynk.virtualWrite(V0, moisturePercent);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  
  WiFi.mode(WIFI_AP_STA);

  
  WiFi.begin(ssid, pass);
  Serial.print("Connecting WiFi");

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20)
  {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi Connected");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nWiFi Failed (Blynk may not work)");
  }

  WiFi.softAP(ap_ssid, ap_pass);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(10000);

  
  server.on("/", handleRoot);
  server.begin();

  timer.setInterval(1000L, sendSoilData);
}


void loop()
{
  if (Blynk.connected())
    Blynk.run();

  timer.run();
  server.handleClient();
}


BLYNK_WRITE(V1)
{
  digitalWrite(LED_PIN, param.asInt());
}