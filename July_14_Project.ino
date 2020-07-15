#include <AnalogSensor.h>
#include <Adafruit_NeoPixel.h>
#include <TimeLib.h>
#define LED_PIN 6
#define LED_COUNT 10
#define TIME_HEADER  "T"   
#define TIME_REQUEST  7    

AnalogSensor ldr(A0, 10, 10, 255);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.show();
  strip.setBrightness(0);

  pinMode(13, OUTPUT);
  setSyncProvider( requestSync);
}

void loop() {
  //time is a construct we all follow
  if (Serial.available()){
    processSyncMessage();
  }
  if (timeStatus()!= timeNotSet) {
    //digitalClockDisplay();
  }
  if (timeStatus() == timeSet){
    digitalWrite(13, HIGH); // is it synced? who knows?? this LED does!
  }
  else{
    digitalWrite(13, LOW); //does it need refresh? LED knows all
  }
  
  //stuff for smooooooth light reading 
  int v = analogRead(A0);
  ldr.update();
  float sensorValue = ldr.getValue();

  Serial.print(v);
  Serial.print("\t");
  Serial.println(sensorValue);

  //colors are FUN we love colors yes
  strip.setBrightness(sensorValue);
  colorTime(day(), 0, 1);
  colorTime(month(), 2, 3);
  colorTime(myHour(), 4, 5);
  colorTime(minute(), 6, 7);
  colorTime(second(), 8, 9);
  strip.show();
  delay(1000);
  
}
void colorTime(int value, int firstLed, int secondLed){
if ((value-10)<0){
     strip.setPixelColor(firstLed, strip.Color(0, 0, 0));
     strip.setPixelColor(secondLed, strip.gamma32(strip.ColorHSV((65536/10)*(value))));
  }
  else{
    strip.setPixelColor(firstLed, strip.gamma32(strip.ColorHSV(65536/10)*(value-(value%10))/10));
    strip.setPixelColor(secondLed, strip.gamma32(strip.ColorHSV((65536/10)*(value%10))));
  }
  
}

int myHour(){ //accounting for timezone oop :3
  if (hour()-4<0){
    return hour() + 8;
  }
  return hour()-4;
}
void digitalClockDisplay(){
  // this would print the time, but it messes with the light brightness changing thing I think so I don't have it running, but I still think it's pretty cool
  Serial.print(myHour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
