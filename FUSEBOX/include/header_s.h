#pragma once

String title = "CE_ST_Fusebox";
String versionDate = "10.05.2021";
String version = "version 1.0ST";
String brainName = String("BrFUSE");
String relayCode = String("FUS");
const unsigned long heartbeatFrequency = 5000;

char *secret_password = (char *)"2517";

const int fuse_hysteris_margin = 3;
const unsigned long timespan_fuseCheck = 50;

#define DEBUG_MODE 					0
// onBoardLED
#define ON_BOARD_LED_PIN            13
#define IGNORE_KEYPAD               0
// #define LCD_DISABLE     1
// LED
// PIN

enum PWM_PIN {
  PWM_1_PIN = 3,                           // Predefined by STB design
  PWM_2_PIN = 5,                           // Predefined by STB design
  PWM_3_PIN = 6,                           // Predefined by STB design
  PWM_4_PIN = 9,                           // Predefined by STB design
};
// SETTINGS
// I2C ADRESSES
#define RELAY_I2C_ADD     	 0x3F         // Relay Expander
#define OLED_I2C_ADD         0x3C         // Predefined by hardware
#define LCD_I2C_ADD					 0x27 // Predefined by hardware
#define KEYPAD_I2C_ADD       0x38         // Keypad
#define FUSE_I2C_ADD         0x39         // Fuses

// RELAY
// PIN
enum REL_PIN {
  REL_1_PIN ,                              // 0 Fusebox lid
  REL_2_PIN ,                              // 1 Door opener / Alarm light
  REL_3_PIN ,                              // 2
  REL_4_PIN ,                              // 3
  REL_5_PIN ,                              // 4
  REL_6_PIN ,                              // 5
  REL_7_PIN ,                              // 6
  REL_8_PIN ,                              // 7 12v PS
};
// AMOUNT
#define REL_AMOUNT               3

// INIT
enum REL_INIT {
  REL_1_INIT   =                0,        // COM-12V_IN, NO-12V_OUT, NC-/  set to 1 for magnet, 0 for mechanical
  REL_2_INIT   =                1,        // COM-12V_IN, NO-12V_OUT_DOOR, NC-12V_OUT_ALARM
  REL_3_INIT   =                1,        // NC-12V_OUT_ALARM
  REL_4_INIT   =                1,        // DESCRIPTION OF THE RELAY WIRING
  REL_5_INIT   =                1,        // DESCRIPTION OF THE RELAY WIRING
  REL_6_INIT   =                1,        // DESCRIPTION OF THE RELAY WIRING
  REL_7_INIT   =                1,        // DESCRIPTION OF THE RELAY WIRING
  REL_8_INIT   =                1,        // COM AC_volt, NO 12_PS+, NC-/
};

// INPUT
enum INPUT_PIN{
  INPUT_1_PIN,                             //  0 alarm light at the entrance/exit
  INPUT_2_PIN,                             //  1 trigger for starting the procedure
  INPUT_3_PIN,                             //  2
  INPUT_4_PIN,                             //  3
  INPUT_5_PIN,                             //  4
  INPUT_6_PIN,                             //  5
  INPUT_7_PIN,                             //  6
  INPUT_8_PIN
};
// AMOUNT
#define FUSE_COUNT             5    // same as input_count in older code
