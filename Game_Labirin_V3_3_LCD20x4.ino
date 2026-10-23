/*
 Game Labirin Interaktif V2.0 (Draft)
 - Arduino Mega
 - 2 Servo MG995
 - 2 Joystick
 - LCD I2C 16x2
 - Tombol START
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,20,4);

Servo servo1,servo2;

//================ PIN =================
const byte JOY1=A0;
const byte JOY2=A2;
const byte SERVO1_PIN=9;
const byte SERVO2_PIN=10;
const byte BTN_START=2;
const byte BTN_RESET=3;
const byte BUZZER=8;
const byte SENSOR_GOAL=5;
const byte LED_WIN=13;

//================ SERVO =================
//================ SERVO =================
const unsigned long servoInterval = 7;
const int langkah = 5;

int pos1=90,pos2=90;
int target1=90,target2=90;
unsigned long prevServo=0;
bool gameAktif=false;
bool gameMenang=false;
unsigned long startTime=0;
float lastTimeSec=0;

//================ LCD =================
const unsigned long splash1Time=2000;
const unsigned long splash2Time=2000;
const unsigned long scrollSpeed=250;
const unsigned long blinkSpeed=500;

char pesan[]="   Letakkan kelereng di posisi START   ";
const int panjang=sizeof(pesan)-1;
byte scrollIndex=0;

unsigned long prevScroll=0;
unsigned long prevBlink=0;
bool blink=true;

//================ STATE =================
enum State{
SHOW_NAME,
SHOW_NIM,
WAIT_START,
SHOW_READY,
COUNT3,
COUNT2,
COUNT1,
GO_STATE,
PLAY,
WIN_WAIT
};

State state=SHOW_NAME;
unsigned long stateMillis=0;

void setup(){

  lcd.init();
  lcd.clear();
  lcd.backlight();

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  servo1.write(90);
  servo2.write(90);

  pinMode(BTN_START,INPUT_PULLUP);
  pinMode(BTN_RESET,INPUT_PULLUP);
  pinMode(BUZZER,OUTPUT);
  pinMode(SENSOR_GOAL,INPUT);
  pinMode(LED_WIN,OUTPUT);
  digitalWrite(LED_WIN,LOW);
  digitalWrite(BUZZER,LOW);

  stateMillis=millis();

  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("PRASETIO FAFINDO");
  lcd.setCursor(3,2);
  lcd.print("21101152610117");
}

void bacaJoystick(){

  if(!gameAktif) return;

  target1=map(analogRead(JOY1),0,1023,104,80);
  target2=map(analogRead(JOY2),0,1023,80,100);
}

void updateServo(){

  if(!gameAktif) return;

  if(millis()-prevServo>=servoInterval){

    prevServo=millis();

    if(pos1<target1){
      pos1+=langkah;
      if(pos1>target1) pos1=target1;
    }else if(pos1>target1){
      pos1-=langkah;
      if(pos1<target1) pos1=target1;
    }

    if(pos2<target2){
      pos2+=langkah;
      if(pos2>target2) pos2=target2;
    }else if(pos2>target2){
      pos2-=langkah;
      if(pos2<target2) pos2=target2;
    }

    servo1.write(pos1);
    servo2.write(pos2);
  }
}

void runningText(){

 if(millis()-prevScroll>=scrollSpeed){

  prevScroll=millis();

  lcd.setCursor(0,1);

  for(byte i=0;i<20;i++)
    lcd.print(pesan[(scrollIndex+i)%panjang]);

  scrollIndex++;
  if(scrollIndex>=panjang) scrollIndex=0;
 }
}

void blinkStart(){

 if(millis()-prevBlink>=blinkSpeed){

  prevBlink=millis();

  blink=!blink;

  lcd.setCursor(4,2);

  if(blink)
    lcd.print("Tekan START");
  else
    lcd.print("           ");
 }
}



void beep(unsigned int freq, unsigned int dur){
  tone(BUZZER,freq,dur);
}

void resetGame(){

  beep(2000,80);
  delay(60);
  beep(2000,80);
  delay(90);

  gameAktif=false;
  gameMenang=false;
  digitalWrite(LED_WIN,LOW);
  pos1=90;
  pos2=90;
  target1=90;
  target2=90;
  servo1.write(90);
  servo2.write(90);

  scrollIndex=0;
  blink=true;
  prevScroll=millis();
  prevBlink=millis();

  lcd.clear();
  state=WAIT_START;
}

void loop(){

  if(digitalRead(BTN_RESET)==LOW){
    delay(30);
    if(digitalRead(BTN_RESET)==LOW){
      resetGame();
      while(digitalRead(BTN_RESET)==LOW);
    }
  }


  bacaJoystick();
  updateServo();

  switch(state){

    case SHOW_NAME:

      if(millis()-stateMillis>=splash1Time){

        lcd.clear();
        lcd.setCursor(2,1);
        lcd.print("SISTEM KOMPUTER");

        state=SHOW_NIM;
        stateMillis=millis();
      }

    break;

    case SHOW_NIM:

      if(millis()-stateMillis>=splash2Time){

        lcd.clear();

        state=WAIT_START;
      }

    break;

    case WAIT_START:

      runningText();
      blinkStart();

      if(digitalRead(BTN_START)==LOW){

        lcd.clear();
        lcd.setCursor(4,1);
        lcd.print("GAME DIMULAI");
        lcd.setCursor(6,2);
        lcd.print("BERSIAP");

        state=SHOW_READY;
        stateMillis=millis();
      }

    break;

    case SHOW_READY:

      if(millis()-stateMillis>=1000){

        lcd.clear();
        lcd.setCursor(9,1);
        lcd.print("3");
        beep(2000,100);

        state=COUNT3;
        stateMillis=millis();
      }

    break;

    case COUNT3:

      if(millis()-stateMillis>=1000){

        lcd.clear();
        lcd.setCursor(9,1);
        lcd.print("2");
        beep(2000,100);

        state=COUNT2;
        stateMillis=millis();
      }

    break;

    case COUNT2:

      if(millis()-stateMillis>=1000){

        lcd.clear();
        lcd.setCursor(9,1);
        lcd.print("1");
        beep(2000,100);

        state=COUNT1;
        stateMillis=millis();
      }

    break;

    case COUNT1:

      if(millis()-stateMillis>=1000){

        lcd.clear();
        lcd.setCursor(8,1);
        lcd.print("GO!!");
        beep(2000,700);

        state=GO_STATE;
        stateMillis=millis();
      }

    break;

    case GO_STATE:

      if(millis()-stateMillis>=700){

        lcd.clear();
        lcd.setCursor(7,1);
        lcd.print("AYO!!");
        gameAktif=true;
        startTime=millis();
        state=PLAY;
      }

    break;

    case PLAY:

      if(gameAktif && !gameMenang){
        if(digitalRead(SENSOR_GOAL)==LOW){   // ubah ke HIGH jika modul Anda aktif HIGH
          delay(100);
          if(digitalRead(SENSOR_GOAL)==LOW){
            gameAktif=false;
            gameMenang=true;
            digitalWrite(LED_WIN,HIGH);

            lastTimeSec=(millis()-startTime)/1000.0;
            lcd.clear();
            lcd.setCursor(6,1);
            lcd.print("SELAMAT!");
            lcd.setCursor(4,2);
            lcd.print("ANDA MENANG");

            tone(BUZZER,1047,150); delay(180);
            tone(BUZZER,1319,150); delay(180);
            tone(BUZZER,1568,150); delay(180);
            tone(BUZZER,2093,350);
            
            delay(2000);

            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("Waktu:");
            lcd.setCursor(0,2);
            lcd.print(lastTimeSec,2);
            lcd.print(" Detik");
            delay(2000);

            lcd.clear();
            if(lastTimeSec<15){
              lcd.setCursor(7,1); lcd.print("HEBAT!");
              lcd.setCursor(4,2); lcd.print("SANGAT CEPAT");
            }else if(lastTimeSec<=30){
              lcd.setCursor(7,1); lcd.print("BAGUS!");
              lcd.setCursor(6,2); lcd.print("TERUS LATIHAN");
            }else{
              lcd.setCursor(6,1); lcd.print("SELESAI!");
              lcd.setCursor(3,2); lcd.print("TERUS LATIHAN");
            }
            delay(2000);

            state=WIN_WAIT;
            stateMillis=millis();
          }
        }
      }

    break;

    case WIN_WAIT:

      if(millis()-stateMillis>=4000){
        resetGame();
      }

    break;

  }

}
