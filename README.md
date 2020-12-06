# NixieClock 

Project outlined in this video: https://youtu.be/dowCgFqBnNU

This is code written in the Arduino IDE to be put on a standalone ATMEGA328P (I made the mistake of getting the ATMEGA328 which can still work, you just need to follow the instructions in the relevant screenshot to change the expected signature)

The board is supplied +5V through a USB connector. There is an MCP7940 RTC for keeping time, an Si7006 for measuring temperature and humidity, an audio peak detector circuit to monitor if someone has clapped, 4 push buttons and it all outputs to 6 LED-based Nixie Tubes. Each of these "Nixie Tubes" has 10 daisy chained WS2812b RGB LEDs, with each one lighting up a separate digit. They are daisy chained in order of the numbers they light up (0, 1, 2... 9).

# Current functionality of the code

Displays current time

Will respond to two successive claps by cycling through the temperature, humidity and date

Does a fade out/fade in when changing which type of information it is presenting

Can long press the SET button to change the time/date

Current thing you are "SETTING" will be highlited by a pulsing different colour and can be manipulated with the UP and DOWN buttons and confirmed by a short press of the SET button

MODE button eventually will have different colour modes. As of now it just lets you change the default orange colour to whatever hue and saturation value you want

# Libraries

I just have a functional barebones understanding of github/arduino so I'm not sure how to properly package and credit libraries and their creators. I've included the zip folders which I used to install the libraries to the Arduino IDE. The one thing is I needed to modify the MCP7940 library to be able to increment/decrement the month and year which wasn't possible with the library I used (I think because those are two varying units of time), so the nonzipped files are what I replaced what was added when I installed the original ones. You'll need to replace the default files if you want to compile the NixieClock.ino sketch

# TempIndicator

Decided to modify the project to make a 2 digit temperature indicator. The code is currently just in a slapped together state built off the clock code where it just checks the temperature every second and displays it. It has a cold blue colour until the temperature goes over 35 where it'll switch to a warmer orange colour
