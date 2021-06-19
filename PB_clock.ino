unsigned long lastTimestamp;
unsigned long int secondCounter;
unsigned long int lastReset;
unsigned long int timerDuration = 3*60; //seconds

unsigned int state = 0; // 0 = counting down, 1 = flashing 00:00. More may appear, hence int.
const unsigned int second = 1000; //1000 millis in a second, less for testing.

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(115200);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  // Clear the buffer
  display.clearDisplay();

  displayTime(timerDuration);
  
  //define button
  pinMode(9, INPUT_PULLUP);
}

void loop() {
  const unsigned long currentMillis = millis();

  if(digitalRead(9)==LOW){
    //Poor quality debounce below
    
    unsigned int count_low = 1; //count number of times we read LOW
    for(int i = 0; i < 100; i++){ // Try 10 more button readings
      if(digitalRead(9)==LOW){
        count_low++;  
      }
      delay(5); //5 milis * 100 extra readings = 500ms additional wait. 
    }
    if(count_low > 90){ // 90 out of 101 button low readings. Seems to work ok. 
      state = 0; // reset the timer.
      
      secondCounter = 0;
      lastReset = millis();

      displayTime(timerDuration);
    }
  }
  if(currentMillis >= secondCounter * second + lastReset){
    // Count downwards, and update display.
    secondCounter++;
    if(state==0){
      if(timerDuration>=secondCounter){
        const unsigned long timeRemaining = timerDuration-secondCounter;
  
        displayTime(timeRemaining);
  
      }else{
        state = 1;
      }
    }else if(state==1){ //blink 00:00
      if(secondCounter % 2 == 0){
        display.clearDisplay();
        display.display();
      }else{
        displayTime(0);
      }
    }
  }
}

void displayTime(unsigned long seconds){
  
  //calc mins and seconds
  const unsigned long minutes = floor(seconds/60);
  seconds = seconds - minutes * 60;
  String clockTime = "";

  clockTime = clockTime + minutes + ":";
  if(seconds <= 9){
    clockTime += "0";
    clockTime += seconds;
  }else{
    clockTime +=  seconds;
  }

  Serial.println(clockTime);

  //get display ready.
  display.setTextSize(5);                
  display.setTextColor(SSD1306_WHITE); 
  display.clearDisplay();
  display.setCursor(0,0);

  //print the time
  display.println(clockTime);
  display.display();
}
