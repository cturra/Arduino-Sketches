#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"
#include "Wire.h"
#include "RTClib.h"

// no-cost stream operator as described at 
// http://sundial.org/arduino/?page_id=119
template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }

static uint8_t MAC[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };

#define DEBUG
#define PORT     80
#define PREFIX   "/fan"
#define POWERPIN 7

RTC_DS1307 rtc;

WebServer webserver(PREFIX, PORT);
int fanState = 0;
DateTime fanActionTime; //datetime when the fan was last turned on/off.


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

        DateTime now = rtc.now();
        fanActionTime = now.unixtime();
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
      fanStatus = "on"; 
    }
    else 
    { 
      fanStatus = "off"; 
    }

    int fanActionHour = fanActionTime.hour();
    int fanActionDay  = fanActionTime.day();
    String fanActionMin   = getLessThanZeroStr((int)fanActionTime.minute());
    String fanActionSec   = getLessThanZeroStr((int)fanActionTime.second());
    String fanActionMonth = getMonthStr((int)fanActionTime.month());
    
    server << "</p> &nbsp; > Status: fan was turned " << fanStatus << " " << fanActionMonth << " " << fanActionDay << " at " << fanActionHour << ":" << fanActionMin << "." << fanActionSec << "</p><br />";
    
    server << "<form action='/fan' method='POST'>";
    server << "<button name='fan' value='1'>Fan On</button>";
    server << "<button name='fan' value='0'>Fan Off</button>";
    server << "</form>";
  
    server.printP(htmlFoot);
  }
}

// function to return full month String from month integer.
String getMonthStr(int month_i)
{
  String month_s;
  if (month_i == 1)
  {
    month_s = "January";
  }
  else if (month_i == 2)
  {
    month_s = "February";
  }
  else if (month_i == 3)
  {
    month_s = "March";
  }
  else if (month_i == 4)
  {
    month_s = "April";
  }
  else if (month_i == 5)
  {
    month_s = "May";
  }
  else if (month_i == 6)
  {
    month_s = "June";
  }
  else if (month_i == 7)
  {
    month_s = "July";
  }
  else if (month_i == 8)
  {
    month_s = "August";
  }
  else if (month_i == 9)
  {
    month_s = "September";
  }
  else if (month_i == 10)
  {
    month_s = "October";
  }
  else if (month_i == 11)
  {
    month_s = "November";
  }
  else if (month_i == 12)
  {
    month_s = "December";
  }
  return month_s;
}

// funtion to prepend a zero to numbers less than 10. 
String getLessThanZeroStr(int time_i)
{
  String time_s;
  if (time_i < 10)
  {
    time_s = "0"+(String)time_i;
  }
  else
  {
    time_s = (String)time_i;
  }
  return time_s;
}

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(MAC);
  pinMode(POWERPIN, OUTPUT);

  Wire.begin();
  rtc.begin(); 
  if (! rtc.isrunning()) 
  {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
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
