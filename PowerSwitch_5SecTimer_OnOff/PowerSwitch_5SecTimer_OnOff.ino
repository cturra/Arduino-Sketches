
#define POWERPIN 7

int i     = 0; // a counter
int sleep = 5000; // number of milliseconds to delay (sleep)

void setup()
{              
  // initialize the digital pin as an output.
  Serial.begin(9600);
  pinMode(POWERPIN, OUTPUT); 
  Serial.println("Setup has ended, entering loop()");
}

void loop() 
{
  // check if i is even
  if ((i%2) == 0)
  {
    digitalWrite(POWERPIN, HIGH);
    Serial.println("Switch On");
  }
  else 
  {
    digitalWrite(POWERPIN, LOW);
    Serial.println("Switch Off");
  }

  i++;
  delay(sleep);
}
