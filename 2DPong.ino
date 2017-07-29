// LED STUFF
const int arrayLenght = 9;          // size of the array below because c++ doesnt have array.lenght (SHITTY LANGUAGE)
int LEDS[] = {10, 9, 8, 7, 6, 5, 4, 3, 2};
int currentLED;                     // The LED which is currently supposed to be on
int lastLED = 0;

long interval = 500;                // the variable that controls the speed of the game
const long defaultInterval = 500;   // this variable holds the deafult value of the interval for resetting the games state
unsigned long previousMillis = 0;

// BUTTON STUFF
const int button = 11;              // right button
const int button2 = 12;             // left button
int buttonState;
int buttonState2;

unsigned long previousMillisButton = 0;
unsigned long previousMillisButton2 = 0;
const long intervalButton = 10;

// BUZZER
const int buzzer = 13;
int buzzerState = 0;

// VALUES
int loopNum = 0; // 
bool dir;                           // direction bool - false=left - true=right
bool gameRunning = false;
bool gamePlayed = false;
bool winner = true;                 // winner bool - false=left - true=right

// TIMER FOR GAMEOVER STATE
unsigned long previousMillisEndTimer = 0;

void setup() {
    // Start the serial port for debugging
    Serial.begin(9600);
    pinMode(button, INPUT);
    pinMode(button2, INPUT);
    pinMode(buzzer, OUTPUT);
    // Set each LED in the LEDS array to OUTPUT mode
    for(int i = 0; i < arrayLenght; i++) {
        pinMode(LEDS[i], OUTPUT);
    }
}

boolean debounceButton(boolean state) {
    boolean stateNow = digitalRead(button);
    if(state!=stateNow) {
        //delay(10);
        unsigned long currentMillisButton = millis();
        if(currentMillisButton - previousMillisButton >= intervalButton) {
            previousMillisButton = currentMillisButton;
            stateNow = digitalRead(button);
        }
    }
    return stateNow;
}

boolean debounceButton2(boolean state2) {
    boolean stateNow2 = digitalRead(button2);
    if(state2!=stateNow2) {
        //delay(10);
        unsigned long currentMillisButton2 = millis();
        if(currentMillisButton2 - previousMillisButton2 >= intervalButton) {
            previousMillisButton2 = currentMillisButton2;
            stateNow2 = digitalRead(button2);
        }
    }
    return stateNow2;
}

void increaseSpeed() {
    // Increase the speed of the game by 25%
    interval = interval * 0.75; 
}

void playAudio(int state) {
    // State 0 is button being pressed
    // State 1 is gameOver music
    // buzzerState dictates the frequency of the tone
    if(state == 0){
        tone(buzzer, random(150, 200), 50);
    } else if(state == 1) {
        switch (buzzerState) {
            case 0:
                tone(buzzer, 500, 50);
                buzzerState++;
                break;
            case 1:
                tone(buzzer, 400, 50);
                buzzerState++;
                break;
            case 2:
                tone(buzzer, 300, 50);
                buzzerState++;
                break;
            default:
                tone(buzzer, 200, 50);
                buzzerState = 0;
                break;
        }
    }
}

void game() {
    /*  If the right button is pressed and the currentLED one of the two last lights to the right
        Change the direction and increase the speed of the game
        TODO: debounceButton function should be rewritten to debounceButton(buttonState, Button)
        TODO: and it should return TRUE if the button has been pressed and debounced and FALSE if it hasnt
        TODO:   if(debounceButton(buttonState, button)) {
                    increaseSpeed();
                    dir = true;
                    playAudio(0);
                }
    */
    if(debounceButton(buttonState) == HIGH && buttonState == LOW) {
        if (!dir && currentLED == 9 || currentLED == 10) {
            increaseSpeed();
            dir = true;
            playAudio(0);
        }
        buttonState = HIGH;
    } else if(debounceButton(buttonState) == LOW && buttonState == HIGH) {
        buttonState = LOW;
    }
    if(debounceButton2(buttonState2) == HIGH && buttonState2 == LOW) {
        if (!dir && currentLED == 2 || currentLED == 3) {
            increaseSpeed();
            dir = false;
            playAudio(0);
        }
        buttonState2 = HIGH;
    } else if(debounceButton2(buttonState2) == LOW && buttonState2 == HIGH) {
        buttonState2 = LOW;
    }

    currentLED = LEDS[loopNum];         // set the current LED
    
        digitalWrite(currentLED, HIGH); // turn on the current LED
        digitalWrite(lastLED, LOW);     // turn off the the last LED

        unsigned long currentMillis = millis();
    
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            if(dir) {   // Move to the right
                loopNum++;
                // If loopNum goes higher than the amount of LEDS then right side wins
                if(loopNum >= arrayLenght) {
                    loopNum = 0;
                    gameRunning = false;
                    winner = false;
                    gamePlayed = true;
                    digitalWrite(currentLED, LOW);
                    previousMillisEndTimer = millis();
                }
            } else {  // Move to the left
                loopNum--;
                // If loopNum goes below the amount of LEDS then left side wins
                if(loopNum < 0) {
                    loopNum = 8;
                    gameRunning = false;
                    winner = true;
                    gamePlayed = true;
                    digitalWrite(currentLED, LOW);
                    previousMillisEndTimer = millis();
                }
            }
          
            lastLED = currentLED;
        }
}

void gameOver() {
    interval = defaultInterval;
    unsigned long currentMillis = millis();
    unsigned long currentMillisEndTimer = millis();
    int celebrationStrobe = 500;
    if (currentMillis - previousMillis >= celebrationStrobe) {
        if(winner){
            digitalWrite(LEDS[8], !digitalRead(LEDS[8]));
        } else {
            digitalWrite(LEDS[0], !digitalRead(LEDS[0]));
        }
        previousMillis = currentMillis;
        playAudio(1);
    }
    // After 10 seconds or if button is pressed then go to preGame state
    // prevent accidental extra button presses from ending gameOver state prematurely
    if (currentMillisEndTimer - previousMillisEndTimer >= 500) {
        if (currentMillisEndTimer - previousMillisEndTimer >= 10000) {
            overToPre();
        }
        if(debounceButton(buttonState) == HIGH && buttonState == LOW) {
            overToPre();
            buttonState = HIGH;
        } else if(debounceButton(buttonState) == LOW && buttonState == HIGH) {
            buttonState = LOW;
        }
        if(debounceButton2(buttonState2) == HIGH && buttonState2 == LOW) {
            overToPre();
            buttonState2 = HIGH;
        } else if(debounceButton2(buttonState2) == LOW && buttonState2 == HIGH) {
            buttonState2 = LOW;
        }
    }
}

void overToPre() {
    gamePlayed = false;
    digitalWrite(LEDS[0], LOW);
    digitalWrite(LEDS[8], LOW);
}

void preGame() {
    digitalWrite(LEDS[0], HIGH);
    digitalWrite(LEDS[8], HIGH);
    if(debounceButton(buttonState) == HIGH && buttonState == LOW) {
        gameRunning = true;
        buttonState = HIGH;
        dir = true;     // Set direction to right
        loopNum = 0;    // Set the starting LED to far left
        digitalWrite(LEDS[0], LOW);
        digitalWrite(LEDS[8], LOW);
    } else if(debounceButton(buttonState) == LOW && buttonState == HIGH) {
        buttonState = LOW;
    }
    if(debounceButton2(buttonState2) == HIGH && buttonState2 == LOW) {
        gameRunning = true;
        buttonState2 = HIGH;
        dir = false; // Set direction to left
        loopNum = 8; // Set the starting LED to far right
        digitalWrite(LEDS[0], LOW);
        digitalWrite(LEDS[8], LOW);
    } else if(debounceButton2(buttonState2) == LOW && buttonState2 == HIGH) {
        buttonState2 = LOW;
    }
}

void loop() {
    if(gameRunning && !gamePlayed) {
        game();
    } else if(!gameRunning && gamePlayed) {
        gameOver();
    } else {
        preGame();
    }
}