#include <TTSi7006.h>
#include <FastLED.h>
#include <MCP7940.h>

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
#define CLAP_MIN_TIME     200 //ms
#define CLAP_MAX_TIME     1000 //ms
#define SET_TIMEOUT       30000 // 30s timeout if no activity


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
#define BRIGHTNESS        200

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
TimeSpan timeChange(0);

TTSi7006 si7006 = TTSi7006(true);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
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
  MCP7940.adjust();                                                           // Set to library compile Date/Time //
  Serial.println(F("Enabling battery backup mode"));                          //                                  //
  MCP7940.setBattery(true);                                                   // enable battery backup mode       //
  then = MCP7940.now();
  now = then;

  Serial.print("Si7006 is connected: ");
  Serial.println(si7006.isConnected() ? "Yes" : "No");

  LEDS.addLeds<LED_TYPE, DIN_L1_PIN, COLOR_ORDER>(leds[DIN_L1], NUM_LEDS);
  LEDS.addLeds<LED_TYPE, DIN_L2_PIN, COLOR_ORDER>(leds[DIN_L1], NUM_LEDS);
  LEDS.addLeds<LED_TYPE, DIN1_PIN, COLOR_ORDER>(leds[DIN1], NUM_LEDS);
  LEDS.addLeds<LED_TYPE, DIN2_PIN, COLOR_ORDER>(leds[DIN2], NUM_LEDS);
  LEDS.addLeds<LED_TYPE, DIN_R1_PIN, COLOR_ORDER>(leds[DIN_R1], NUM_LEDS);
  LEDS.addLeds<LED_TYPE, DIN_R2_PIN, COLOR_ORDER>(leds[DIN_R2], NUM_LEDS);

  FastLED.setBrightness(BRIGHTNESS);

}

void loop() {
  currentStateSET = digitalRead(SW_SET_PIN);
  currentStateMODE = digitalRead(SW_MODE_PIN);
  currentStateATHRESH = digitalRead(ATHRESH_PIN);
  if(digitalRead(SW_UP_PIN))
    Serial.println("UP");
  if(digitalRead(SW_DOWN_PIN))
    Serial.println("DOWN");

  //SET Button - Long or short press
  if(lastStateSET == LOW && currentStateSET == HIGH)        // button is pressed
    pressedTimeSET = millis();
  else if(lastStateSET == HIGH && currentStateSET == LOW) { // button is released
    releasedTimeSET = millis();

    long pressDuration = releasedTimeSET - pressedTimeSET;

    if( pressDuration < SHORT_PRESS_TIME )
      Serial.println("SET - SHORT PRESS");
    else
      Serial.println("SET - LONG PRESS");
  }

  if(lastStateMODE == LOW && currentStateMODE == HIGH)    // button is pressed
    Serial.println("MODE - BUTTON PRESS");

  //Audio Spike - Did a double clap happen?
  if(lastStateATHRESH == LOW && currentStateATHRESH == HIGH) {        // Sound happens
    currentClap = millis();
    Serial.println("Audio Spike");
    if(currentClap - lastClap > CLAP_MIN_TIME && currentClap - lastClap < CLAP_MAX_TIME) {
      Serial.println("THE CLAPPER HAS HAPPENED"); //This is where you would call a function to display temperature
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

  now = MCP7940.now();
  if(now.second() != then.second()) {
    then = now;
    printTime();                                            // Display the current date/time    //
  }


  // save the the last state
  lastStateSET = currentStateSET;
  lastStateMODE = currentStateMODE;
  lastStateATHRESH = currentStateATHRESH;

}

void setShortPress() {
  if(setTimeIndex > 0) {
    setTimeIndex++;
    //Serial specific
    switch(setTimeIndex) {
      case 1:
        Serial.println("Set Hour");
        timeChange = TimeSpan(0,1,0,0);
        break;
      case 2:
        Serial.println("Set Minute");
        timeChange = TimeSpan(0,0,1,0);
        break;
      case 3:
        Serial.println("Set Second");
        timeChange = TimeSpan(0,0,0,1);
        break;
      case 4:
        Serial.println("Set Month");
        break;
      case 5:
        Serial.println("Set Day");
        timeChange = TimeSpan(1,0,0,0);
        break;
      case 6:
        Serial.println("Set Year");
        break;
      case 7:
        Serial.println("Finished Setting");
        setTimeIndex = 0;
        break;
    }

  }
}

void setLongPress() {
  if(setTimeIndex == 0)
    setTimeIndex = 1;
    Serial.println("Set Hour");
    timeChange = TimeSpan(0,1,0,0);
  else
  setTimeIndex = 0;
}

void modePress() {
  
}

void printTime() {
  sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),          // Use sprintf() to pretty print    //
            now.month(), now.day(), now.hour(), now.minute(), now.second());  // date/time with leading zeros     //
  Serial.println(inputBuffer);                                              // Display the current date/time    //
}
