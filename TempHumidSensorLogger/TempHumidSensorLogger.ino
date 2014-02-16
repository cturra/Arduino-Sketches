#include <DHT.h>
#include <RTClib.h>
#include <SD.h>
#include <Wire.h>

#define DHTTYPE DHT21 /* DHT 21 (AM2301) */
RTC_DS1307 rtc;
const int CHIP_SELECT = 4; // ethernet shield is pin 4

DHT dht1(2, DHTTYPE);
DHT dht2(3, DHTTYPE);

const char* LOG = "sensor.csv"; //log file

int total_delay = 0;
boolean DEBUG   = true;


void setup() {
  Serial.begin(9600);  //Begin serial communcation
  
  /** setup sensors **/
  dht1.begin();
  dht2.begin();

  /** setup clock module **/
  Wire.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
  rtc.begin();

  if (! rtc.isrunning()) {
    if (DEBUG) {
      Serial.println("RTC is NOT running!");
    }
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  /** setup sd card **/
  pinMode(10, OUTPUT);

  if (!SD.begin(CHIP_SELECT)) {
    if (DEBUG) {
      Serial.println("SD Card failed or is not present");
    }
  }
}

void loop() {
  DateTime now = rtc.now();
  
  // do this every 20 minutes
  if ((now.minute()%20) == 0) {
    char tmp[10];
    String data_str = "";
    
    float sensor1_temp1 = readTemp(dht1);
    delay(250); // can take up to 250ms to read sensor
    total_delay += 250;
    float sensor1_temp2 = readTemp(dht1);
    float sensor1_temp_avg = ((sensor1_temp1+sensor1_temp2)/2);
    
    float sensor2_temp1 = readTemp(dht2);
    delay(250); // can take up to 250ms to read sensor
    total_delay += 250;
    float sensor2_temp2 = readTemp(dht2);
    float sensor2_temp_avg = ((sensor2_temp1+sensor2_temp1)/2);
    
    float temp_diff = abs(sensor2_temp_avg-sensor1_temp_avg);
    dtostrf(temp_diff,1,2,tmp);
    data_str += (String)tmp+", ";
  
    float temp_diff_pcnt = abs((sensor2_temp_avg-sensor1_temp_avg)/((sensor2_temp_avg+sensor1_temp_avg)/2))*100;
    dtostrf(temp_diff_pcnt,1,2,tmp);
    data_str += (String)tmp+", ";
    
    float sensor1_hum1 = readHum(dht1);
    delay(250); // can take up to 250ms to read sensor
    total_delay += 250;
    float sensor1_hum2 = readHum(dht1);
    float sensor1_hum_avg = ((sensor1_hum1+sensor1_hum2)/2);

    float sensor2_hum1 = readHum(dht2);
    delay(250); // can take up to 250ms to read sensor
    total_delay += 250;
    float sensor2_hum2 = readHum(dht2);
    float sensor2_hum_avg = ((sensor2_hum1+sensor2_hum2)/2);
    
    float hum_diff = abs(sensor1_hum_avg-sensor2_hum_avg);
    dtostrf(hum_diff,1,2,tmp);
    data_str += (String)tmp+", ";
  
    float hum_diff_pcnt = abs((sensor1_hum_avg-sensor2_hum_avg)/((sensor1_hum_avg+sensor2_hum_avg)/2))*100;
    dtostrf(hum_diff_pcnt,1,2,tmp);
    data_str += (String)tmp;
    
    if (DEBUG) {
      Serial.println("log => "+data_str);
    }
    
    log_write(data_str);
  }
  
  // sleep for one (1) minute
  delay((long)(60000-total_delay));
}

float readTemp(DHT dht) {
  return dht.readTemperature();
}

float readHum(DHT dht) {
  return dht.readHumidity();
}

/** print data to SD card **/
void log_write(String log_data) {
  File file = SD.open(LOG, FILE_WRITE);

  if (file) {
    file.println(log_data);
    file.close();
  }
  
  else{
    if (DEBUG) {
      Serial.println("SD: error opening file.");
    }
  }
}
