/***************************************************************************************************************//*!
* @file MCP7940.h
*
*  @mainpage Arduino Library Header for the MCP7940M and MCP7940N Real-Time Clock devices
*
*  @section MCP7940_intro_section Description
*
* Class definition header for the MCP7940 from Microchip. Both the MCP7940N (with battery backup pin= and the
* MCP7940M are supported with this library and they both use the same I2C address. This chip is a Real-Time-Clock
* with an I2C interface. The data sheet located at http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf
* describes the functionality used in this library.\n
* Use is made of portions of Adafruit's RTClib Version 1.2.0 at https://github.com/adafruit/RTClib which is a
* a fork of the original RTClib from Jeelabs. The re-used code encompasses only the classes for time and date.\n
*
* @section license GNU General Public License v3.0
* This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
* option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details. You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @section author Author
*
* Written by Arnd\@SV-Zanshin
*
* @section versions Changelog
* 
* Version| Date       | Developer           | Comments
* ------ | ---------- | ------------------- | --------
* 1.1.7  | 2020-05-18 | wvmarle             | Issue #47 - adjust() method with compiler variables fix
* 1.1.7  | 2019-02-07 | davidlehrian        | Issue #43 - Calibration might cause overflow of "trim" variable
* 1.1.7  | 2019-02-07 | davidlehrian        | Issue #42 - calibrate(DateTime) should correct the current date/time
* 1.1.6  | 2019-01-27 | davidlehrian        | Issue #36 - Reopened, changed assignment statements to avoid warning
* 1.1.5  | 2019-01-19 | SV-Zanshin          | Issue #40 - Change commenting structure and layout to use doxygen
* 1.1.5  | 2019-01-19 | INemesisI           | Issue #37 - Corrected AlarmPolarity bit clearing on setAlarm()
* 1.1.5  | 2019-01-19 | SV-Zanshin          | Issue #39 - Small changes to remove compiler warnings for Travis-CI
* 1.1.5  | 2019-01-18 | INemesisI           | Issue #38 - Overflow on setting new TRIM value
* 1.1.4  | 2018-12-15 | hexeguitar          | Issue #36 - Overflow on I2C_MODES datatype corrected
* 1.1.4  | 2018-12-15 | huynh213            | Issue #35 - MCP7940.adjust() function resets the PWRFAIL and VBATEN
* 1.1.3  | 2018-08-08 | amgrays             | Issue #32 - invalid return on SetMFP corrected
* 1.1.3  | 2018-08-05 | HannesJo0139        | Issue #31 - incorrect calibration trim on negative numbers
* 1.1.2  | 2018-08-04 | SV-Zanshin          | Issue #28 - added new calibrate() overload for frequency calibration
* 1.1.2  | 2018-07-08 | logicaprogrammabile | Issue #26 - regarding hour bitmasks
* 1.1.1  | 2018-07-07 | SV-Zanshin          | Fixed bugs introduced by 14, 20, and 21
* 1.1.1  | 2018-07-07 | wvmarle             | Pull #21  - Additional changes
* 1.1.1  | 2018-07-06 | wvmarle             | Pull #20  - Numerous changes and enhancements
* 1.1.0  | 2018-07-05 | wvmarle             | Pull #14  - bug fixes to alarm state and cleaned up comments
* 1.0.8  | 2018-07-02 | SV-Zanshin          | Added guard code against multiple I2C constant definitions
* 1.0.8  | 2018-06-30 | SV-Zanshin          | Enh #15   - Added I2C Speed selection
* 1.0.7  | 2018-06-21 | SV-Zanshin          | Issue #13 - DateTime.dayOfTheWeek() is 0-6 instead of 1-7
* 1.0.6  | 2018-04-29 | SV-Zanshin          | Issue  #7 - Moved setting of param defaults to prototypes
* 1.0.6  | 2018-04-29 | SV-Zanshin          | Issue #10 - incorrect setting of alarm with WKDAY to future date
* 1.0.5b | 2017-12-18 | SV-Zanshin          | Issue  #8 - incorrect setting to 24-Hour clock
* 1.0.5a | 2017-10-31 | SV-Zanshin          | Issue  #6 - to remove classification on 2 template functions
* 1.0.4c | 2017-08-13 | SV-Zanshin          | Enhancement #5 to remove checks after Wire.requestFrom()
* 1.0.4b | 2017-08-08 | SV-Zanshin          | Replaced readRAM and writeRAM with template functions
* 1.0.4a | 2017-08-06 | SV-Zanshin          | Removed unnecessary MCP7940_I2C_Delay and all references
* 1.0.3  | 2017-08-05 | SV-Zanshin          | Added calls for MCP7940N. getPowerFail(), clearPowerFail(), setBattery(). Added I2C_READ_ATTEMPTS to prevent I2C hang, added getPowerUp/Down()
* 1.0.3a | 2017-07-29 | SV-Zanshin          | Cleaned up comments, no code changes
* 1.0.3  | 2017-07-29 | SV-Zanshin          | Added getSQWSpeed(),setSQWSpeed(),setSQWState() & getSQWState()
* 1.0.2  | 2017-07-29 | SV-Zanshin          | Added getAlarm(),setAlarmState(),getAlarmState() functions and added the optional setting to setAlarm(). Added isAlarm(). Fixed errors with alarm 1 indexing.
* 1.0.1  | 2017-07-25 | SV-Zanshin          | Added overloaded Calibrate() to manually set the trim factor
* 1.0.0  | 2017-07-23 | SV-Zanshin          | Cleaned up code, initial github upload
* 1.0.2b | 2017-07-20 | SV-Zanshin          | Added alarm handling
* 1.0.1b | 2017-07-19 | SV-Zanshin          | Added methods
* 1.0.0b | 2017-07-17 | SV-Zanshin          | Initial coding
*******************************************************************************************************************/

#include "Arduino.h"  // Arduino data type definitions
#include <Wire.h>     // Standard I2C "Wire" library
#ifndef MCP7940_h     // Guard code definition
  /** @brief  Guard code definition */
  #define MCP7940_h   // Define the name inside guard code
  /*****************************************************************************************************************
  ** Declare classes used in within the class                                                                     **
  *****************************************************************************************************************/
  class TimeSpan;
  /*****************************************************************************************************************
  ** Declare constants used in the class                                                                          **
  *****************************************************************************************************************/
  #ifndef I2C_MODES   // I2C related constants
    /** @brief Guard code definition */
    #define I2C_MODES // Guard code to prevent multiple definitions
    const uint32_t I2C_STANDARD_MODE      =     100000; ///< Default normal I2C 100KHz speed
    const uint32_t I2C_FAST_MODE          =     400000; ///< Fast mode
  #endif
  #if !defined(BUFFER_LENGTH) // The ESP32 Wire library doesn't currently define BUFFER_LENGTH
    /** @brief If the "Wire.h" library doesn't define the buffer, do so here */
    #define BUFFER_LENGTH 32
  #endif
  const uint8_t  MCP7940_ADDRESS           =      0x6F; ///< Device address, fixed value
  const uint8_t  MCP7940_RTCSEC            =      0x00; ///< Timekeeping, RTCSEC Register address 
  const uint8_t  MCP7940_RTCMIN            =      0x01; ///< Timekeeping, RTCMIN Register address 
  const uint8_t  MCP7940_RTCHOUR           =      0x02; ///< Timekeeping, RTCHOUR Register address 
  const uint8_t  MCP7940_RTCWKDAY          =      0x03; ///< Timekeeping, RTCWKDAY Register address 
  const uint8_t  MCP7940_RTCDATE           =      0x04; ///< Timekeeping, RTCDATE Register address 
  const uint8_t  MCP7940_RTCMTH            =      0x05; ///< Timekeeping, RTCMTH Register address 
  const uint8_t  MCP7940_RTCYEAR           =      0x06; ///< Timekeeping, RTCYEAR Register address 
  const uint8_t  MCP7940_CONTROL           =      0x07; ///< Timekeeping, RTCCONTROL Register address 
  const uint8_t  MCP7940_OSCTRIM           =      0x08; ///< Timekeeping, RTCOSCTRIM Register address 
  const uint8_t  MCP7940_ALM0SEC           =      0x0A; ///< Alarm 0, ALM0SEC Register address
  const uint8_t  MCP7940_ALM0MIN           =      0x0B; ///< Alarm 0, ALM0MIN Register address
  const uint8_t  MCP7940_ALM0HOUR          =      0x0C; ///< Alarm 0, ALM0HOUR Register address
  const uint8_t  MCP7940_ALM0WKDAY         =      0x0D; ///< Alarm 0, ALM0WKDAY Register address
  const uint8_t  MCP7940_ALM0DATE          =      0x0E; ///< Alarm 0, ALM0DATE Register address
  const uint8_t  MCP7940_ALM0MTH           =      0x0F; ///< Alarm 0, ALM0MTH Register address
  const uint8_t  MCP7940_ALM1SEC           =      0x11; ///< Alarm 1, ALM1SEC Register address
  const uint8_t  MCP7940_ALM1MIN           =      0x12; ///< Alarm 1, ALM1MIN Register address
  const uint8_t  MCP7940_ALM1HOUR          =      0x13; ///< Alarm 1, ALM1HOUR Register address
  const uint8_t  MCP7940_ALM1WKDAY         =      0x14; ///< Alarm 1, ALM1WKDAY Register address
  const uint8_t  MCP7940_ALM1DATE          =      0x15; ///< Alarm 1, ALM1DATE Register address
  const uint8_t  MCP7940_ALM1MTH           =      0x16; ///< Alarm 1, ALM1MONTH Register address
  const uint8_t  MCP7940_PWRDNMIN          =      0x18; ///< Power-Fail, PWRDNMIN Register address
  const uint8_t  MCP7940_PWRDNHOUR         =      0x19; ///< Power-Fail, PWRDNHOUR Register address
  const uint8_t  MCP7940_PWRDNDATE         =      0x1A; ///< Power-Fail, PWDNDATE Register address
  const uint8_t  MCP7940_PWRDNMTH          =      0x1B; ///< Power-Fail, PWRDNMTH Register address
  const uint8_t  MCP7940_PWRUPMIN          =      0x1C; ///< Power-Fail, PWRUPMIN Register address
  const uint8_t  MCP7940_PWRUPHOUR         =      0x1D; ///< Power-Fail, PWRUPHOUR Register address
  const uint8_t  MCP7940_PWRUPDATE         =      0x1E; ///< Power-Fail, PWRUPDATE Register address
  const uint8_t  MCP7940_PWRUPMTH          =      0x1F; ///< Power-Fail, PWRUPMTH Register address
  const uint8_t  MCP7940_RAM_ADDRESS       =      0x20; ///< NVRAM - Start address for SRAM
  const uint8_t  MCP7940_ST                =         7; ///< MCP7940 register bits. RTCSEC reg
  const uint8_t  MCP7940_12_24             =         6; ///< RTCHOUR, PWRDNHOUR & PWRUPHOUR
  const uint8_t  MCP7940_AM_PM             =         5; ///< RTCHOUR, PWRDNHOUR & PWRUPHOUR
  const uint8_t  MCP7940_OSCRUN            =         5; ///< RTCWKDAY register
  const uint8_t  MCP7940_PWRFAIL           =         4; ///< RTCWKDAY register
  const uint8_t  MCP7940_VBATEN            =         3; ///< RTCWKDAY register
  const uint8_t  MCP7940_LPYR              =         5; ///< RTCMTH register
  const uint8_t  MCP7940_OUT               =         7; ///< CONTROL register
  const uint8_t  MCP7940_SQWEN             =         6; ///< CONTROL register
  const uint8_t  MCP7940_ALM1EN            =         5; ///< CONTROL register
  const uint8_t  MCP7940_ALM0EN            =         4; ///< CONTROL register
  const uint8_t  MCP7940_EXTOSC            =         3; ///< CONTROL register
  const uint8_t  MCP7940_CRSTRIM           =         2; ///< CONTROL register
  const uint8_t  MCP7940_SQWFS1            =         1; ///< CONTROL register
  const uint8_t  MCP7940_SQWFS0            =         0; ///< CONTROL register
  const uint8_t  MCP7940_SIGN              =         7; ///< OSCTRIM register
  const uint8_t  MCP7940_ALMPOL            =         7; ///< ALM0WKDAY register
  const uint8_t  MCP7940_ALM0IF            =         3; ///< ALM0WKDAY register
  const uint8_t  MCP7940_ALM1IF            =         3; ///< ALM1WKDAY register
  const uint32_t SECONDS_PER_DAY           =     86400; ///< 60 secs * 60 mins * 24 hours
  const uint32_t SECONDS_FROM_1970_TO_2000 = 946684800; ///< Seconds between year 1970 and 2000
  /*************************************************************************************************************//*!
  * @class   DateTime
  * @brief   Simple general-purpose date/time class
  * @details Copied from RTClib. For further information on this implementation see 
  *          https://github.com/SV-Zanshin/MCP7940/wiki/DateTimeClass
  *****************************************************************************************************************/
  class DateTime 
  {
    public:
      DateTime (uint32_t t=0);
      DateTime (uint16_t year,uint8_t month,uint8_t day,uint8_t hour=0, uint8_t min=0,uint8_t sec=0);
      DateTime (const DateTime& copy);
      DateTime (const char* date, const char* time);
      DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time);
      /*! return the current year */
      uint16_t year()         const { return 2000 + yOff; }
      /*! return the current month */
      uint8_t  month()        const { return m; }
      /*! return the current day of the month */
      uint8_t  day()          const { return d; }
      /*! return the current hour */
      uint8_t  hour()         const { return hh; }
      /*! return the current minute */
      uint8_t  minute()       const { return mm; }
      /*! return the current second */
      uint8_t  second()       const { return ss; }
	  //increase year by 1
	  void	   incYear()	  { yOff++; }
	  //decrease year by 1
	  void	   decYear()	  { yOff--; }
	  //increase month by 1
	  void	   incMonth();
	  //decrease month by 1
	  void	   decMonth();
      /*! return the current day of the week starting at 0 */
      uint8_t  dayOfTheWeek() const;
      /*! return the current seconds in the year */
      long     secondstime()  const;
      /*! return the current Unixtime */
      uint32_t unixtime(void) const;
      /*! Overloaded "+" operator to add two timespans */
      DateTime operator+(const TimeSpan& span);
      /*! Overloaded "+" operator to add two timespans */
      DateTime operator-(const TimeSpan& span);
      /*! Overloaded "-" operator subtract add two timespans */
      TimeSpan operator-(const DateTime& right);
    protected:
      uint8_t yOff; ///< Internal year offset value
      uint8_t    m; ///< Internal month value
      uint8_t    d; ///< Internal day value
      uint8_t   hh; ///< Internal hour value
      uint8_t   mm; ///< Internal minute value
      uint8_t   ss; ///< Internal seconds
  }; // of class DateTime definition
  /*************************************************************************************************************//*!
  * @class   TimeSpan
  * @brief   Timespan class which can represent changes in time with seconds accuracy
  * @details Copied from RTClib. For further information see
  *          https://github.com/SV-Zanshin/MCP7940/wiki/TimeSpanClass for additional details
  *****************************************************************************************************************/
  class TimeSpan 
  {
    public:
      TimeSpan (int32_t seconds = 0);                                        ///< Default constructor
      TimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds); ///< Overloaded constructor
      TimeSpan (const TimeSpan& copy);                                       ///< Overloaded constructor
      int16_t  days() const         { return _seconds / 86400L; }            ///< return the number of days
      int8_t   hours() const        { return _seconds / 3600 % 24; }         ///< return number of hours
      int8_t   minutes() const      { return _seconds / 60 % 60; }           ///< return number of minutes
      int8_t   seconds() const      { return _seconds % 60; }                ///< return number of seconds
      int32_t  totalseconds() const { return _seconds; }                     ///< return total number of seconds
      TimeSpan operator+(const TimeSpan& right);                             ///< redefine "+" operator
      TimeSpan operator-(const TimeSpan& right);                             ///< redefine "-" operator
    protected:
      int32_t _seconds;                                                      ///< Internal value for total seconds
  }; // of class TimeSpan definition

  /*************************************************************************************************************//*!
  * @class MCP7940_Class
  * @brief Main class definition with forward declarations
  *****************************************************************************************************************/
  class MCP7940_Class 
  {
    public:
      MCP7940_Class() {};  ///< Unused Class constructor
      ~MCP7940_Class() {}; ///< Unused Class destructor
      bool     begin(const uint32_t i2cSpeed = I2C_STANDARD_MODE);
      bool     deviceStatus();
      bool     deviceStart();
      bool     deviceStop();
      DateTime now();
      void     adjust();
      void     adjust(const DateTime& dt);
      int8_t   calibrate();
      int8_t   calibrate(const int8_t newTrim);
      int8_t   calibrate(const DateTime& dt);
      int8_t   calibrate(const float MeasuredFrequency);
      int8_t   getCalibrationTrim();
      uint8_t  weekdayRead();
      uint8_t  weekdayWrite(const uint8_t dow);
      bool     setMFP(const bool value);
      uint8_t  getMFP();
      bool     setAlarm(const uint8_t alarmNumber, const uint8_t alarmType, const DateTime dt, const bool state = true );
      void     setAlarmPolarity(const bool polarity);
      DateTime getAlarm(const uint8_t alarmNumber, uint8_t &alarmType);
      bool     clearAlarm(const uint8_t alarmNumber);
      bool     setAlarmState(const uint8_t alarmNumber, const bool state);
      bool     getAlarmState(const uint8_t alarmNumber);
      bool     isAlarm(const uint8_t alarmNumber);
      uint8_t  getSQWSpeed();
      bool     setSQWSpeed(uint8_t frequency, bool state = true);
      bool     setSQWState(const bool state);
      bool     getSQWState();
      bool     setBattery(const bool state);
      bool     getBattery();
      bool     getPowerFail();
      bool     clearPowerFail();
      DateTime getPowerDown();
      DateTime getPowerUp();
	  int8_t   calibrateOrAdjust(const DateTime& dt);
	  int32_t  getPPMDeviation(const DateTime& dt);
	  void     setSetUnixTime(uint32_t aTime);
	  uint32_t getSetUnixTime();
/*******************************************************************************************************************
** Declare the readRAM() and writeRAM() methods as template functions to use for all I2C device I/O. The code has **
** to be in the main library definition rather than the actual MCP7940.cpp library file.The template functions    **
** allow any type of data to be read and written, be it a byte or a character array or a structure.               **
**                                                                                                                **
** The MCP7940 supports 64 bytes of general purpose SRAM memory, which can be used to store data. For more        **
** details, see datasheet page 36.                                                                                **
**                                                                                                                **
** The data is stored in a block of 64 bytes, reading beyond the end of the block causes the address pointer to   **
** roll over to the start of the block.                                                                           **
*******************************************************************************************************************/

/***************************************************************************************************************//*!
* @brief     Template for readRAM()
* @details   As a template it can support compile-time data type definitions
* @param[in] addr Memory address
* @param[in] value    Data Type "T" to read
* @return    Pointer to return data structure
*******************************************************************************************************************/
      template< typename T >
      uint8_t&  readRAM(const uint8_t addr, T &value) 
      {
        uint8_t* bytePtr    = (uint8_t*)&value;            // Pointer to structure beginning
        uint8_t  structSize = sizeof(T);                   // Number of bytes in structure
        uint8_t  i         = 0;                            // loop counter
        Wire.beginTransmission(MCP7940_ADDRESS);           // Address the I2C device
        Wire.write((addr%64) + MCP7940_RAM_ADDRESS);       // Send register address to write
        _TransmissionStatus = Wire.endTransmission();      // Close transmission
        for (i=0;i<structSize;i++)                         // loop for each byte to be read
        {
          if (i%BUFFER_LENGTH==0)                          // loop for each buffer block
          { 
            Wire.requestFrom(MCP7940_ADDRESS, structSize); // Request a block of data
          } // of if-then we are at a buffer boundary
          *bytePtr++ = Wire.read(); // next byte
        } // of for-next each byte to be read
        return (i);
      } // of method readRAM()
/***************************************************************************************************************//*!
* @brief     Template for writeRAM()
* @details   As a template it can support compile-time data type definitions
* @param[in] addr Memory address
* @param[in] value Data Type "T" to write
* @return    True if successful, otherwise false
*******************************************************************************************************************/
      template<typename T>
      bool writeRAM(const uint8_t addr, const T &value) 
      {
        const uint8_t* bytePtr = (const uint8_t*)&value; // Pointer to structure beginning
        Wire.beginTransmission(MCP7940_ADDRESS);         // Address the I2C device
        Wire.write((addr%64) + MCP7940_RAM_ADDRESS);     // Send register address to write
        for (uint8_t i = 0; i < sizeof(T); i++)          // loop for each byte to be written
        {
          Wire.write(*bytePtr++);
        } // of for-next each byte
        _TransmissionStatus = Wire.endTransmission();    // Close transmission
        return (!_TransmissionStatus);                   // return error status
      } // of method writeRAM()
    private:
      uint8_t  readByte(const uint8_t addr);                         // Read 1 byte from address on I2C
      void     writeByte(const uint8_t addr, const uint8_t data);    // Write 1 byte at address to I2C
      uint8_t  bcd2int(const uint8_t bcd);                           // convert BCD digits to integer
      uint8_t  int2bcd(const uint8_t dec);                           // convert integer to BCD
      uint8_t  _TransmissionStatus = 0;                              ///< Status of I2C transmission
      bool     _CrystalStatus     = false;                           ///< True if RTC is turned on
      bool     _OscillatorStatus  = false;                           ///< True if Oscillator on and working
      uint32_t _SetUnixTime       = 0;                               ///< UNIX time when clock last set
      uint8_t  _ss,                                                  ///< Time component Seconds
               _mm,                                                  ///< Time component Minutes
               _hh,                                                  ///< Time component Hours
               _d,                                                   ///< Time component Days
               _m;                                                   ///< Time component Months
      uint16_t _y;                                                   ///< Time component Years
      void     clearRegisterBit(const uint8_t reg, const uint8_t b); // Clear a bit, values 0-7
      void     setRegisterBit  (const uint8_t reg, const uint8_t b); // Set   a bit, values 0-7
      void     writeRegisterBit(const uint8_t reg, const uint8_t b,  // Clear a bit, values 0-7
                                bool bitvalue);                      //                                  //
      uint8_t  readRegisterBit (const uint8_t reg, const uint8_t b); // Read  a bit, values 0-7
  }; // of MCP7940 class definition
#endif