#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

#define CLOCKPIN D1
#define DATAPIN_LEFT D2
#define DATAPIN_RIGHT D0

const char *ssid = "GuidingSmombies";
const char *password = "wifiwifi";

ESP8266WebServer server(80);
IPAddress myIP;

Adafruit_DotStarMatrix left = Adafruit_DotStarMatrix( 
  8, 8, DATAPIN_LEFT, CLOCKPIN,
  DS_MATRIX_TOP     + DS_MATRIX_RIGHT +
  DS_MATRIX_COLUMNS + DS_MATRIX_PROGRESSIVE,
  DOTSTAR_BGR
);

Adafruit_DotStarMatrix right = Adafruit_DotStarMatrix( 
  8, 8, DATAPIN_RIGHT, CLOCKPIN,
  DS_MATRIX_TOP     + DS_MATRIX_LEFT +
  DS_MATRIX_COLUMNS + DS_MATRIX_PROGRESSIVE,
  DOTSTAR_BGR
);

bool pActive = false;
int pStimuli = 1;
int pDisplay = 0;
int pDelay = 1;
int pHeight = 4;
int pBrightness = 8;
int pColor[] = {255, 0, 0};

int MAX_ITERATION = 16;
int MAX_ITERATION_HALF = MAX_ITERATION / 2;
int iteration = 0;

int MAX_LED = 8;
int led = 0;

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for(int i=0; i<=maxIndex && found<=index; i++)
    if(data.charAt(i)==separator || i==maxIndex)
    {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void handleRoot()
{
  String message = "DotX&nbsp;incoming request...<br />";

  left.clear();
  right.clear();

  iteration = 0;
  led = 0;

  for (int i = 0; i < server.args(); i++) 
  {
    if(server.argName(i).equals("active"))
    {
      pActive = server.arg(i).toInt() == 1 ? true : false;
      message += "Changed active to " + (String)pActive + " ...<br />";
    }
    else if(server.argName(i).equals("stimuli"))
    {
      pStimuli = server.arg(i).toInt();
      message += "Changed stimuli to " + (String)pStimuli + " ...<br />";
    }
    else if(server.argName(i).equals("display"))
    {
      pDisplay = server.arg(i).toInt();
      message += "Changed display to " + (String)pDisplay + " ...<br />";
    }
    else if(server.argName(i).equals("delay"))
    {
      pDelay = server.arg(i).toInt();
      message += "Changed delay to " + (String)pDelay + " ...<br />";
    }
    else if(server.argName(i).equals("height"))
    {
      pHeight = server.arg(i).toInt();
      message += "Changed height to " + (String)pHeight + " ...<br />";
    }
    else if(server.argName(i).equals("brightness"))
    {
      pBrightness = server.arg(i).toInt();
      left.setBrightness(pBrightness);
      right.setBrightness(pBrightness);
      message += "Changed brightness to " + (String)pBrightness + " ...<br />";
    }
    else if(server.argName(i).equals("color"))
    {
      String color = server.arg(i);
      if(color.length() >= 5)
      {
        pColor[0] = getValue(color, ',', 0).toInt();
        pColor[1] = getValue(color, ',', 1).toInt();
        pColor[2] = getValue(color, ',', 2).toInt();
      }
      message += "Changed color to " + color + " ...<br />";
    }
  } 
      
  server.send(200, "text/html", message);
}

void setup() 
{
  delay(1000);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("GuidingSmombies starting up...");
  Serial.println();

  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP();

  Serial.print("Listening to IP ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.begin();

  delay(1000);

  left.begin();
  right.begin();
  
  left.clear();
  right.clear();

  left.setBrightness(pBrightness);
  right.setBrightness(pBrightness);
  
  left.show();
  right.show();
}

void loop()
{
  server.handleClient();
  delay(pDelay);

  if(pActive)
  {
    if(pStimuli == 1)
    {
      if(pDisplay == -1 || pDisplay == 0)
        left.drawPixel(led, pHeight, left.Color(pColor[0], pColor[1], pColor[2]));
      if(pDisplay == 1 || pDisplay == 0)
        right.drawPixel(led, pHeight, right.Color(pColor[0], pColor[1], pColor[2]));
    }
  
    else if(pStimuli == 2)
    {
      if(++iteration > MAX_ITERATION)
        iteration = 0;
      
      float i = (MAX_ITERATION_HALF - abs(MAX_ITERATION_HALF - iteration));
      i /= MAX_ITERATION_HALF;
      
      if(pDisplay == -1 || pDisplay == 0)
        left.drawPixel(led, pHeight, left.Color((int)(pColor[0] * i), (int)(pColor[1] * i), (int)(pColor[2] * i)));
      if(pDisplay == 1 || pDisplay == 0)
        right.drawPixel(led, pHeight, right.Color((int)(pColor[0] * i), (int)(pColor[1] * i), (int)(pColor[2] * i)));
    }

    else if(pStimuli == 3)
    {
      iteration += 4;
      if(iteration > MAX_ITERATION)
      {
        iteration = 0;
        if(++led > MAX_LED)
          led = 0;
      }
      
      if(pDisplay == -1 || pDisplay == 0)
      {
        left.drawPixel(led - 1, pHeight, left.Color(0, 0, 0));
        left.drawPixel(led, pHeight, left.Color((int)(pColor[0]), (int)(pColor[1]), (int)(pColor[2])));
      }
      if(pDisplay == 1 || pDisplay == 0)
      {
        right.drawPixel(led - 1, pHeight, right.Color(0, 0, 0));
        right.drawPixel(led, pHeight, right.Color((int)(pColor[0]), (int)(pColor[1]), (int)(pColor[2])));
      }
    }
  
    else if(pStimuli == 4)
    {
      iteration += 4;
      if(iteration > MAX_ITERATION)
      {
        iteration = 0;
        if(++led > MAX_LED)
          led = 0;
      }
        
      float i = iteration;
      i /= MAX_ITERATION;
      float j = 1 - i;
      
      if(pDisplay == -1 || pDisplay == 0)
      {
        left.drawPixel(led - 1, pHeight, left.Color((int)(pColor[0] * j), (int)(pColor[1] * j), (int)(pColor[2] * j)));
        left.drawPixel(led, pHeight, left.Color((int)(pColor[0] * i), (int)(pColor[1] * i), (int)(pColor[2] * i)));
      }
      if(pDisplay == 1 || pDisplay == 0)
      {
        right.drawPixel(led - 1, pHeight, right.Color((int)(pColor[0] * j), (int)(pColor[1] * j), (int)(pColor[2] * j)));
        right.drawPixel(led, pHeight, right.Color((int)(pColor[0] * i), (int)(pColor[1] * i), (int)(pColor[2] * i)));
      }
    }
  } 
  else 
  {
    left.fillScreen(left.Color(0,0,0));
    right.fillScreen(right.Color(0,0,0));
  }

  left.show();
  right.show();
}
