
/*
 * Nixie Clock Project
 * Colin Fraser
 * August 21, 2020
 * 
 * Notes: Pin 0/1 were a bad choice since they are shared by usart. Can't use serial if writing to R1/R2
 */


#include <TTSi7006.h>
#include <FastLED.h>
#include <MCP7940.h>
#include <math.h>

//constants
//const uint32_t  BAUD_RATE     = 115200;
//const uint8_t   SW_UP_PIN     = 8;
//const uint8_t   SW_DOWN_PIN   = 10;
//const uint8_t   SW_SET_PIN    = A1;
//const uint8_t   SW_MODE_PIN   = 9;
//const uint8_t   ATHRESH_PIN   = 3;
//const int       SHORT_PRESS_TIME = 500; // 500ms
//const int       CLAP_MIN_TIME = 200; //200 ms
//const int       CLAP_MAX_TIME = 1000; //1s
#define BAUD_RATE         115200
#define SW_UP_PIN         8
#define SW_DOWN_PIN       10
#define SW_SET_PIN        A1
#define SW_MODE_PIN       9
#define ATHRESH_PIN       3
#define SHORT_PRESS_TIME  500 //ms
#define UPDOWN_COOLDOWN   200 //ms
#define CLAP_MIN_TIME     200 //ms
#define CLAP_MAX_TIME     1000 //ms
#define SET_TIMEOUT       30000 // 30s timeout if no activity

// Non numerical LED locations
//#define TEMP_SYMB       0 //needs updating
#define MINUS_SYMB      9 //needs updating
#define CELS_SYMB       6 //needs updating
#define FAHR_SYMB       9 //needs updating
#define KELV_SYMB       8 //needs updating
#define RH_SYMB         6 //needs updating
#define PCNT_SYMB       7 //needs updating


//Trim value set to -180 clock cycles every minute


#define NUM_LEDS          10
#define NUM_STRIPS        6

#define DIN_L1_PIN        6 // Which pin this is hooked up to
#define DIN_L1            0 // Index in the array of where this is kept

#define DIN_L2_PIN        7
#define DIN_L2            1

#define DIN1_PIN          5
#define DIN1              2

#define DIN2_PIN          4
#define DIN2              3

#define DIN_R1_PIN        0
#define DIN_R1            4

#define DIN_R2_PIN        1
#define DIN_R2            5

#define LED_TYPE          WS2812B
#define COLOR_ORDER       GRB
#define MAX_BRIGHTNESS        255

// Define the 2D array of LEDs and strips
CRGB leds[NUM_STRIPS][NUM_LEDS];

const uint8_t  SPRINTF_BUFFER_SIZE =     32;                                  // Buffer size for sprintf()        //

//variables
bool lastStateSET       = LOW;  // the previous state from the SET pin
bool lastStateMODE      = LOW;  // the previous state from the MODE pin
bool lastStateATHRESH   = LOW;  // the previous state from the ATHRESH pin
bool currentStateSET;           // the current reading from the SET pin
bool currentStateMODE;           // the current reading from the SET pin
bool currentStateATHRESH;       // the current reading from the ATHRESH pin
unsigned long pressedTimeSET   = 0;
unsigned long releasedTimeSET = 0;
unsigned long lastClap    = 0;
unsigned long currentClap = 0;
int setTimeIndex = 0;

char          inputBuffer[SPRINTF_BUFFER_SIZE];                               // Buffer for sprintf()/sscanf()    //

MCP7940_Class MCP7940;
DateTime now;
DateTime then;
int currTemp = 23;
int currHumid = 30;
int currUnit = CELS_SYMB;
TimeSpan timeChange(0);

TTSi7006 si7006 = TTSi7006(true);

int brightness = MAX_BRIGHTNESS;
int displayIndex = 0;
int fadeFlag = 0;
int isCycling = 0;
unsigned long lastCycle = 0;

#define CYCLE_PERIOD    4000 //ms

CRGB colours[6];



void setup() {
  // put your setup code here, to run once:
  //Serial.begin(BAUD_RATE); //Using this will make right board (Seconds) stop working
  pinMode(SW_UP_PIN, INPUT);
  pinMode(SW_DOWN_PIN, INPUT);
  pinMode(SW_SET_PIN, INPUT);
  pinMode(SW_MODE_PIN, INPUT);
  pinMode(ATHRESH_PIN, INPUT);

  Serial.println(F("\nStarting NixieClock program version 0.1"));
  Serial.print(F("- Compiled with c++ version "));                            //                                  //
  Serial.print(F(__VERSION__));                                               // Show compiler information        //
  Serial.print(F("\n- On "));                                                 //                                  //
  Serial.print(F(__DATE__));                                                  //                                  //
  Serial.print(F(" at "));                                                    //                                  //
  Serial.print(F(__TIME__));                                                  //                                  //
  Serial.print(F("\n"));     

  while (!MCP7940.begin()) {                                                  // Initialize RTC communications    //
    Serial.println(F("Unable to find MCP7940M. Checking again in 3s."));      // Show error text                  //
    delay(3000);                                                              // wait a second                    //
  } // of loop until device is located 
  Serial.println(F("MCP7940 initialized."));                                  //                                  //
  while (!MCP7940.deviceStatus()) {                                           // Turn oscillator on if necessary  //
    Serial.println(F("Oscillator is off, turning it on."));                   //                                  //
    bool deviceStatus = MCP7940.deviceStart();                                // Start oscillator and return state//
    if (!deviceStatus) {                                                      // If it didn't start               //
      Serial.println(F("Oscillator did not start, trying again."));           // Show error and                   //
      delay(1000);                                                            // wait for a second                //
    } // of if-then oscillator didn't start                                   //                                  //
  } // of while the oscillator is off                                         //                                  //
  //MCP7940.adjust();                                                           // Set to library compile Date/Time //
  Serial.println(F("Enabling battery backup mode"));                          //                                  //
  MCP7940.setBattery(true);                                                   // enable battery backup mode       //
  then = MCP7940.now();
  now = then;

  Serial.print("Si7006 is connected: ");
  Serial.println(si7006.isConnected() ? "Yes" : "No");

  FastLED.addLeds<LED_TYPE, DIN_L1_PIN, COLOR_ORDER>(leds[DIN_L1], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DIN_L2_PIN, COLOR_ORDER>(leds[DIN_L2], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DIN1_PIN, COLOR_ORDER>(leds[DIN1], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DIN2_PIN, COLOR_ORDER>(leds[DIN2], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DIN_R1_PIN, COLOR_ORDER>(leds[DIN_R1], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DIN_R2_PIN, COLOR_ORDER>(leds[DIN_R2], NUM_LEDS);

  for(int i=0; i < 6; i++)
    colours[i] = CRGB::OrangeRed;

  FastLED.setBrightness(brightness);

}

void loop() {
  currentStateSET = digitalRead(SW_SET_PIN);
  currentStateMODE = digitalRead(SW_MODE_PIN);
  currentStateATHRESH = digitalRead(ATHRESH_PIN);
  

  //SET Button - Long or short press
  if(lastStateSET == LOW && currentStateSET == HIGH)        // button is pressed
    pressedTimeSET = millis();
  else if(lastStateSET == HIGH && currentStateSET == LOW) { // button is released
    releasedTimeSET = millis();

    long pressDuration = releasedTimeSET - pressedTimeSET;

    if( pressDuration < SHORT_PRESS_TIME ) 
      setShortPress();//Serial.println("SET - SHORT PRESS");
    else
      setLongPress();//Serial.println("SET - LONG PRESS");
  }

  static bool isBright = 0;
  if(lastStateMODE == LOW && currentStateMODE == HIGH)    // button is pressed
  {
    Serial.println("MODE - BUTTON PRESS");
//    if(isBright)
//    {
//      isBright = 0;
//      FastLED.setBrightness(BRIGHTNESS);
//
//    } else {
//      isBright = 1;
//      FastLED.setBrightness(255);
//    }

  }

  //Audio Spike - Did a double clap happen?
  if(lastStateATHRESH == LOW && currentStateATHRESH == HIGH) {        // Sound happens
    currentClap = millis();
    Serial.println("Audio Spike");
    if(currentClap - lastClap > CLAP_MIN_TIME && currentClap - lastClap < CLAP_MAX_TIME) {
      Serial.println("THE CLAPPER HAS HAPPENED"); //This is where you would call a function to display temperature
      cycleDisplay();
      //Print out humidity
      Serial.print("Humidity: ");
      Serial.print(si7006.readHumidity());
      Serial.println(" % rel.");
    
      //Print out Temperature °C
      Serial.print("Temperature: ");
      Serial.print(si7006.readTemperatureC());
      Serial.print(" ");
      Serial.print(char(176));
      Serial.println("C");
    }

    lastClap = currentClap;
  }
  if(setTimeIndex != 0) {
    if(digitalRead(SW_UP_PIN)) {
      if( setTimeIndex == 4 )
        now.incMonth();
      else if( setTimeIndex == 6 )
        now.incYear();
      else
        now = now + timeChange;
      printTime(); 
      delay(UPDOWN_COOLDOWN);
    } else if(digitalRead(SW_DOWN_PIN)) {
      if( setTimeIndex == 4 )
        now.decMonth();
      else if( setTimeIndex == 6 )
        now.decYear();
      else
        now = now - timeChange;
      printTime(); 
      delay(UPDOWN_COOLDOWN);
    }

    
  } else {

    now = MCP7940.now();
    if(now.second() != then.second()) {
      then = now;
      printTime();                                            // Display the current date/time    //
    }
  }

  //Fade handler
  if(fadeFlag == 1) {
    brightness--;
    if(brightness <= 0) {
      brightness = 0;
      fadeFlag++;
      
      displayIndex++;
      if(displayIndex > 3) {
        displayIndex = 0;
        isCycling = 0;
      } else {
        isCycling = 1;
        lastCycle = millis();
      }
        
    }
    FastLED.setBrightness(brightness);
    
  } else if(fadeFlag == 2) {
    brightness++;
    if(brightness >= MAX_BRIGHTNESS) {
      brightness = MAX_BRIGHTNESS;
      fadeFlag = 0;
    }
    FastLED.setBrightness(brightness);
  }

  if(isCycling) {
    if( millis() - lastCycle >= CYCLE_PERIOD ) 
      cycleDisplay();
  }


  // save the the last state
  lastStateSET = currentStateSET;
  lastStateMODE = currentStateMODE;
  lastStateATHRESH = currentStateATHRESH;

  updateLEDs();

}

void setShortPress() {
  if(setTimeIndex > 0) {
    setTimeIndex++;
    //Serial specific
    switch(setTimeIndex) {
      case 1: //Impossible
        Serial.println("Set Hour");
        timeChange = TimeSpan(0,1,0,0);
        break;
      case 2:
        colours[DIN_L1] = CRGB::OrangeRed;
        colours[DIN_L2] = CRGB::OrangeRed;
        colours[DIN1] = CRGB::Indigo;
        colours[DIN2] = CRGB::Indigo;
        Serial.println("Set Minute");
        timeChange = TimeSpan(0,0,1,0);
        break;
      case 3:
        colours[DIN1] = CRGB::OrangeRed;
        colours[DIN2] = CRGB::OrangeRed;
        colours[DIN_R1] = CRGB::Indigo;
        colours[DIN_R2] = CRGB::Indigo;
        Serial.println("Set Second");
        timeChange = TimeSpan(0,0,0,1);
        break;
      case 4:
        colours[DIN_R1] = CRGB::OrangeRed;
        colours[DIN_R2] = CRGB::OrangeRed;
        colours[DIN_L1] = CRGB::Indigo;
        colours[DIN_L2] = CRGB::Indigo;
        Serial.println("Set Month");
        displayIndex = 3;
        break;
      case 5:
        colours[DIN_L1] = CRGB::OrangeRed;
        colours[DIN_L2] = CRGB::OrangeRed;
        colours[DIN1] = CRGB::Indigo;
        colours[DIN2] = CRGB::Indigo;
        Serial.println("Set Day");
        timeChange = TimeSpan(1,0,0,0);
        break;
      case 6:
        colours[DIN1] = CRGB::OrangeRed;
        colours[DIN2] = CRGB::OrangeRed;
        colours[DIN_R1] = CRGB::Indigo;
        colours[DIN_R2] = CRGB::Indigo;
        Serial.println("Set Year");
        break;
      case 7:
        colours[DIN_R1] = CRGB::OrangeRed;
        colours[DIN_R2] = CRGB::OrangeRed;
        Serial.println("Finished Setting");
        displayIndex = 0;
        setTimeIndex = 0;
        MCP7940.adjust(now);
        break;
    }

  }
}

void setLongPress() {
  if(setTimeIndex == 0) {
    colours[DIN_L1] = CRGB::Indigo;
    colours[DIN_L2] = CRGB::Indigo;
    setTimeIndex = 1;
    Serial.println("Set Hour");
    timeChange = TimeSpan(0,1,0,0);
  } else {
    for(int i=0;i<6;i++)
    colours[i] = CRGB::OrangeRed;
    displayIndex = 0;
    setTimeIndex = 0;
    MCP7940.adjust(now);
  }
}

void modePress() {
  
}

void printTime() {
  sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),          // Use sprintf() to pretty print    //
            now.month(), now.day(), now.hour(), now.minute(), now.second());  // date/time with leading zeros     //
  Serial.println(inputBuffer);                                              // Display the current date/time    //
}

//Kicks off the fade flag which begins cycling through temp/humid/date displays
void cycleDisplay() {
  if(setTimeIndex == 0) { //DO NOT want to start cycling while you're in the middle of setting the time

    
  //Could add color changes here
//  currTemp = round( si7006.readTemperatureC() );
//  currHumid = round( si7006.readHumidity() );
  fadeFlag = 1;

  }
}

//Updates the tube LEDs
void updateLEDs() {
  FastLED.clear();
  switch(displayIndex) {
    case 0: //time
      leds[DIN_L1][now.hour() / 10] = colours[DIN_L1];
      leds[DIN_L2][now.hour() % 10] = colours[DIN_L2];
      leds[DIN1][now.minute() / 10] = colours[DIN1];
      leds[DIN2][now.minute() % 10] = colours[DIN2];
      leds[DIN_R1][now.second() / 10] = colours[DIN_R1];
      leds[DIN_R2][now.second() % 10] = colours[DIN_R2];
      break;
    case 1: //temp
      currTemp = round( si7006.readTemperatureC() );
      if(currTemp < 0)
        leds[DIN_L1][MINUS_SYMB] = colours[DIN_L1];
      if(abs(currTemp) >= 100)
        leds[DIN_L2][abs(currTemp) / 100] = colours[DIN_L2];
      leds[DIN1][(abs(currTemp)/10) % 10] = colours[DIN1];
      leds[DIN2][abs(currTemp) % 10] = colours[DIN2];
      leds[DIN_R1][currUnit] = colours[DIN_R1];
      break;      
    case 2: //humid
      currHumid = round( si7006.readHumidity() );
      leds[DIN_L1][RH_SYMB] = colours[DIN_L1];
      leds[DIN1][currHumid / 10] = colours[DIN1];
      leds[DIN2][currHumid % 10] = colours[DIN2];
      leds[DIN_R1][PCNT_SYMB] = colours[DIN_L1];
      break;
    case 3: //date
      leds[DIN_L1][now.month() / 10] = colours[DIN_L1];
      leds[DIN_L2][now.month() % 10] = colours[DIN_L2];
      leds[DIN1][now.day() / 10] = colours[DIN1];
      leds[DIN2][now.day() % 10] = colours[DIN2];
      leds[DIN_R1][(now.year()/10) % 10] = colours[DIN_R1];
      leds[DIN_R2][now.year() % 10] = colours[DIN_R2];
      break;    
  }
  //blinking effect when setting DONT really like, should switch to an oscillatory brightness
  if(setTimeIndex != 0) {
    if(millis() % 900 > 500) {
      if(setTimeIndex == 1 || setTimeIndex == 4) {
        for(int i=0;i<10;i++) {
          leds[DIN_L1][i] = CRGB(0,0,0);
          leds[DIN_L2][i] = CRGB(0,0,0);
        }
      } else if(setTimeIndex == 2 || setTimeIndex == 5) {
        for(int i=0;i<10;i++) {
          leds[DIN1][i] = CRGB(0,0,0);
          leds[DIN2][i] = CRGB(0,0,0);
        }
      } else {
        for(int i=0;i<10;i++) {
          leds[DIN_R1][i] = CRGB(0,0,0);
          leds[DIN_R2][i] = CRGB(0,0,0);
        }
      }
    }
  }
  FastLED.show();
}
