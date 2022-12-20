/**
 * @file m_Acces.ino
 * @author Martin Pek (martin.pek@web.de)
 * @brief access module supports RFID and Keypad authentication with oled&buzzer feedback
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "header_st.h"                                     
#include <Keypad_I2C.h>
#include <stb_brain.h>
#include <avr/wdt.h>
#include <PCF8574.h> /* https://github.com/skywodd/pcf8574_arduino_library - modifiziert!  */
#include "LiquidCrystal_I2C.h"

#include <Password.h>
#include <stb_keypadCmds.h>
#include <stb_oledCmds.h>


/*
build for lib_arduino 0.6.7 onwards
TODO:
 - periodic updates on the password? everytime its polled? 

🔲✅
Fragen and access module Requirements
    🔲 Oledreset
    🔲 printHeadline


*/


STB_BRAIN Brain;
LiquidCrystal_I2C lcd(LCD_I2C_ADD, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Keypad_I2C Keypad(makeKeymap(KeypadKeys), KeypadRowPins, KeypadColPins, KEYPAD_ROWS, KEYPAD_COLS, KEYPAD_ADD, PCF8574_MODE);

// the Evaluation is done on the Mother, may be kept for convenience or removed later
Password passKeypad = Password(dummyPassword);
unsigned long lastKeypadAction = millis();

unsigned long lastOledAction = millis();
// technically only the 3rd line i will update
bool oledHasContent = false;
// freq is unsinged int, durations are unsigned long
// frequency, ontime, offtime
unsigned int buzzerFreq[BuzzerMaxStages] = {0};
unsigned long buzzerOn[BuzzerMaxStages] = {0};
unsigned long buzzerOff[BuzzerMaxStages] = {0};
unsigned long buzzerTimeStamp = millis();

int buzzerStage = -1;


bool lcdInit() {
    Serial.print(F("HD44780 init..."));
	lcd.begin(20,4);  // 20*4 New LiquidCrystal
	lcd.setCursor(0,1);
	lcd.print("Starting...");

	for (int i =0; i<2; i++) {
		delay(500);
		lcd.noBacklight();
		delay(500);
		lcd.backlight();
	}

	printHomescreen();
    Serial.print(F("done"));
	return true;
}

void setup() {
    
    // starts serial and default oled
    
    Brain.begin();
    Brain.STB_.dbgln("v0.09");
    wdt_enable(WDTO_8S);

    Brain.setSlaveAddr(0);

    // Brain.receiveSetup();
    // for ease of development
    Brain.flags = keypadFlag;
    buzzer_init();
    lcdInit();
    wdt_reset();


    if (Brain.flags & keypadFlag) {
        keypad_init();
    }
 
    // Brain.STB_.printSetupEnd();

}




void printHomescreen() {
    oledHasContent = false;
    lastOledAction = millis();
    lcd.clear();
    lcd.home();
    lcd.setCursor(4,0);
    lcd.print("Enter  Code");
    lcd.setCursor(0,3);
    lcd.print("* Clear / # Confirm");
    lcd.setCursor(6,1);
}


void loop() {

    wdt_reset();
    
    if (Brain.flags & keypadFlag) {
        Keypad.getKey();
    }
    
    
    keypadResetCheck();
    buzzerUpdate();

    if (!Brain.slaveRespond()) {
        return;
    }

    while (Brain.STB_.rcvdPtr != NULL) {
        // Serial.println("Brain.STB_.rcvdPtr");
        interpreter();
        Brain.nextRcvdLn();
    }
}


void interpreter() {
    if (checkForValid()) {return;}
}


// --- Buzzer 

/**
 * @brief Set buzzer timings to be excecuter
 * @param i 
 * @param freq 
 * @param ontime 
 * @param offtime 
*/
void setBuzzerStage(int i, unsigned int freq, unsigned long ontime, unsigned long offtime=0) {
    buzzerFreq[i] = freq;
    buzzerOn[i] = ontime;
    buzzerOff[i] = offtime;
    buzzerStage = 0;
}


void buzzer_init() {
    pinMode(BUZZER_PIN,OUTPUT);
    noTone(BUZZER_PIN);
}


// may change this to contain differen lengts for on and off depending on reqs
void buzzerUpdate() {
    if (buzzerStage < 0) {return;}
    if (buzzerStage >= BuzzerMaxStages) {
        buzzerReset();
        return;
    }

    if (buzzerStage == 0 || millis() > buzzerTimeStamp) {
        // moves next execution to after the on + offtime elapsed 
        buzzerTimeStamp = millis() + buzzerOn[buzzerStage] + buzzerOff[buzzerStage];
        if (buzzerFreq[buzzerStage] > 0) {
            tone(BUZZER_PIN, buzzerFreq[buzzerStage], buzzerOn[buzzerStage]);
            buzzerStage++;
            return;
        } else {
            buzzerReset();
        }
    }
}


void buzzerReset() {
    for (int i=0; i<BuzzerMaxStages; i++) {
        buzzerStage = -1;
        buzzerFreq[i] = 0;
    }
}


// ---


// checks keypad feedback, its only correct/incorrect
bool checkForValid() {

    // Serial.print("checking: ");
    // Serial.println(Brain.STB_.rcvdPtr);
    
    if (strncmp(keypadCmd.c_str(), Brain.STB_.rcvdPtr, keypadCmd.length()) == 0) {
        Brain.sendAck();
        // Serial.println("incoming keypadCmd");
        // do i need a fresh char pts here?
        char *cmdPtr = strtok(Brain.STB_.rcvdPtr, KeywordsList::delimiter.c_str());
        cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
        int cmdNo;
        sscanf(cmdPtr, "%d", &cmdNo);
        wdt_reset();

        if (cmdNo == KeypadCmds::correct) {
            setBuzzerStage(0, 1000, 400, 200);
            setBuzzerStage(1, 1500, 1500, 0);
            lcd.setCursor(3,2);
            lcd.print("Acces Granted");
            delay(5000);
        } else {
            setBuzzerStage(0, 400, 200, 50);
            setBuzzerStage(1, 400, 200, 50);
            setBuzzerStage(2, 400, 200, 50);
            lcd.setCursor(6,2);
            lcd.print("Invalid");
            delay(1500);
        }
        printHomescreen();
        return true;
    }
    return false;
}


// --- Keypad


void keypad_init() {
    Keypad.addEventListener(keypadEvent);  // Event Listener erstellen
    Keypad.begin(makeKeymap(KeypadKeys));
    Keypad.setHoldTime(5000);
    Keypad.setDebounceTime(20);
}


void keypadEvent(KeypadEvent eKey) {
    KeyState state = IDLE;

    state = Keypad.getState();

    if (state == PRESSED) {
        lastKeypadAction = millis();
    }

    switch (state) {
        case PRESSED:
            switch (eKey) {
                case '#':
                    checkPassword();
                    // some beep? or only after mother answers?
                    break;

                case '*':
                    passwordReset();
                    tone(BUZZER_PIN, 261, 400);
                    break;

                default:
                    if (oledHasContent) {return;}
                    passKeypad.append(eKey);
                    if (strlen(passKeypad.guess) >= KEYPAD_CODE_LENGTH_MAX) {
                        checkPassword();
                    }
                    // currently optional and nice to have but other things are prio
                    // Brain.STB_.rs485AddToBuffer(passKeypad.guess);
                    // TODO: probably going to modify this this needs to be centered line 2

                    unsigned int eKeyUInt = (unsigned char) (eKey - '0');
                    Serial.println(eKeyUInt);
                    // STB_OLED::writeToLine(&Brain.STB_.defaultOled, 2, passKeypad.guess, true);
                    lcd.print(eKey);
                    tone(BUZZER_PIN, keypadBaseTone + keypadPitchMultiplier*eKeyUInt, 100);
                    // delay(200);
                    break;
            }
            break;

        default:
            break;
    }
}


/**
 * @brief sends the password to the Mother for evaluation
 */
void checkPassword() {
    if (strlen(passKeypad.guess) < 1) return;

    char msg[20] = ""; 
    strcpy(msg, keypadCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    char noString[3] = "";
    sprintf(noString, "%i", KeypadCmds::evaluate);
    strcat(msg, noString);
    strcat(msg, KeywordsList::delimiter.c_str());
    strcat(msg, passKeypad.guess);
    Serial.println("test");
    delay(500);
    passwordReset();
    lastOledAction = millis();
    Brain.addToBuffer(msg, true);
}


void passwordReset() {
    if (strlen(passKeypad.guess) > 0) {
        passKeypad.reset();
    }
    printHomescreen();
}


void keypadResetCheck() {
    if (millis() - lastKeypadAction > keypadResetInterval) {
        checkPassword();
    }
}


void oledResetCheck() {
    if (!oledHasContent) {return;}
    if (millis() - lastOledAction > keypadResetInterval) {
        printHomescreen();
    }
}



