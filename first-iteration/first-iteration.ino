#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

#define MAX 11

const char *ssid = "GuidingSmombies";
const char *password = "wifiwifi";

ESP8266WebServer server(80);
IPAddress myIP;

Adafruit_NeoPixel left  = Adafruit_NeoPixel(MAX, D4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel right = Adafruit_NeoPixel(MAX, D3, NEO_GRB + NEO_KHZ800);

bool parameter_run = false;
bool parameter_invert = false;
int parameter_mode = 0;
int parameter_delay = 100;
int parameter_color[] = {0, 0, 20};

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for(int i=0; i<=maxIndex && found<=index; i++)
  {
    if(data.charAt(i)==separator || i==maxIndex)
    {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void handleRoot()
{
  String message = "<h1>PeLiCar Wifi AccessPoint</h1><br /><br />";

  for (int i = 0; i < server.args(); i++) 
  {
    if(server.argName(i).equals("run"))
    {
      parameter_run = server.arg(i).toInt() == 1 ? true : false;
      message += "<b>LOG:</b> parameter run received";
    }
    if(server.argName(i).equals("invert"))
    {
      parameter_invert = server.arg(i).toInt() == 1 ? true : false;
      message += "<b>LOG:</b> parameter invert received";
    }
    if(server.argName(i).equals("mode"))
    {
      parameter_mode = server.arg(i).toInt();
      message += "<b>LOG:</b> parameter mode received";
    }
    if(server.argName(i).equals("delay"))
    {
      parameter_delay = server.arg(i).toInt();
      message += "<b>LOG:</b> parameter delay received";
    }
    if(server.argName(i).equals("color"))
    {
      String color = server.arg(i);
      if(color.length() >= 5)
      {
        parameter_color[0] = getValue(color, ',', 0).toInt();
        parameter_color[1] = getValue(color, ',', 1).toInt();
        parameter_color[2] = getValue(color, ',', 2).toInt();
      }
      message += "<b>LOG:</b> parameter color received";
    }
  } 
      
  server.send(200, "text/html", message);
}

void setup() 
{
  delay(1000);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("GuidingSmombies Wifi AccessPoint");
  Serial.println();

  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP();

  Serial.print("IP: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.begin();

  delay(1000);

  left.begin();
  left.clear();
  left.show();

  right.begin();
  right.clear();
  right.show();
}

int pos = 0;

void loop() 
{
  server.handleClient();

  left.clear();
  right.clear();

  if(!parameter_run)
    return;

  delay(parameter_delay);

  if(!parameter_invert)
    pos--;
  else
    pos++;
    
  if(pos < 0)
    pos = MAX - 1;
  if(pos >= MAX)
    pos = 0;

  if(parameter_mode == 0 || parameter_mode == -1)
    left.setPixelColor(pos, parameter_color[0], parameter_color[1], parameter_color[2]);
  if(parameter_mode == 0 || parameter_mode == 1)
    right.setPixelColor(pos, parameter_color[0], parameter_color[1], parameter_color[2]);

  left.show();
  right.show();
}
