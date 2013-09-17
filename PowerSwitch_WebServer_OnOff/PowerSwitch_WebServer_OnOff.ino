#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

// no-cost stream operator as described at 
// http://sundial.org/arduino/?page_id=119
template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }

static uint8_t MAC[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };

#define PORT     80
#define PREFIX   "/fan"
#define POWERPIN 7

WebServer webserver(PREFIX, PORT);
int fanState = 0;

void fanCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      repeat = server.readPOSTparam(name, 16, value, 16);
      if (strcmp(name, "fan") == 0)
      {
        fanState = strtoul(value, NULL, 10);
      }
    } while (repeat);
    
    server.httpSeeOther(PREFIX);
    return;
  }
  
  
  server.httpSuccess();

  if (type == WebServer::GET)
  {
    P(htmlHead) =
      "<html>"
      "<head>"
      "<title>Fan Control</title>"
      "<style type=\"text/css\">"
      "BODY { font-family: Tahoma }"
      "H1 { font-size: 14pt; }"
      "P  { font-size: 10pt; }"
      "</style>"
      "</head>"
      "<body>";
  
     P(htmlFoot) = 
       "</body>"
       "</html>";
  
    server.printP(htmlHead);
 
    server << "<h1>Fan Control</h1>";
    String fanStatus = "";
    if (fanState > 0) 
    { 
      fanStatus = "On"; 
    }
    else 
    { 
      fanStatus = "Off"; 
    }
    server << "</p>Current fan status: " << fanStatus << "</p><br />";
    
    server << "<form action='/fan' method='POST'>";
    server << "<button name='fan' value='1'>Fan On</button>";
    server << "<button name='fan' value='0'>Fan Off</button>";
    server << "</form>";
  
    server.printP(htmlFoot);
  }
}

void setup()
{
  Ethernet.begin(MAC);
  pinMode(7, OUTPUT); 
  webserver.setDefaultCommand(&fanCmd);
  webserver.begin();
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();

  // send power on/of state to PowerSwitch (pin)
  if (fanState == 1)
  {
    digitalWrite(POWERPIN, HIGH);
  }
  else
  {
    digitalWrite(POWERPIN, LOW);
  }
}
