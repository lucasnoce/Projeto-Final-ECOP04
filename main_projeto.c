#include <pic18f4520.h>
#include "bits.h"
#include "config.h"
#include "io.h"
#include "keypad.h"
#include "serial.h"
#include "keypad.h"
#include "timer.h"
#include "lcd.h"
#include "adc.h"

#define BUMBO 40
#define CAIXA 48
#define TOM_1 62
#define TOM_2 63
#define HIH_F 71
#define HIH_A 73
#define RIDE  76
#define CR_1  80
#define CR_2  81

//bits da variável de controle do metronomo (flag)
#define METRON 7
#define LCD    6

unsigned char flag = 0xC0;
static unsigned int bpm = 120;
unsigned int countMetron = 0;
static unsigned char velocity = 0x50;
unsigned int auxvel;

void tocarSom(unsigned char key){
  serialSend(0x90); //note ON event; ch 1
  serialSend(key); //key
  serialSend(velocity); //velocity
  serialSend('\n');
}

void checarTimer(){
  if (timerEnded() && bitTst(flag, METRON)){
    timerReset(10000);
    countMetron++;
    if (countMetron >= (6000/bpm)){
      bitClr(TRISB, 7);
      bitSet(PORTB, 7);
      countMetron = 0;
    }
    else if (countMetron == 20){
      bitClr(TRISB, 7);
      bitClr(PORTB, 7);
    }
  }
}

void checarLCD(){
  if (!bitTst(flag, METRON) && bitTst(flag, LCD)){
    bitClr(TRISB, 7);
    bitClr(PORTB, 7);
    print(-1);
    bitClr(flag, LCD);
  }
  else if (bitTst(flag, METRON) && !bitTst(flag, LCD)){
    print(bpm);
    bitSet(flag, LCD);
  }
}

void checarADC(){
  velocity = adc_amostra() >> 3;
}


void main(void){
  kpInit();
  timerInit();
  serialInit();
  lcdInit();
  
  print(bpm);
  unsigned int valorTeclado; //valor lido pelo telcado
  unsigned char n = 0;

  for(;;){
    switch (n){
      case 0: checarTimer();
              n = 1;
              break;
      case 1: checarLCD();
              n = 2;
              break;
      case 2: checarADC();
              n = 0;
              break;
    }
    
    //leitura do teclado
    kpDebounce();
    valorTeclado = kpRead();
    switch (valorTeclado){
      case 0:    break;                  //nenhuma tecla
      case 8:    tocarSom(BUMBO); break; //tecla 1
      case 128:  tocarSom(CAIXA); break; //tecla 2
      case 2048: tocarSom(TOM_1); break; //tecla 3
      case 4:    tocarSom(TOM_2); break; //tecla 4
      case 64:   tocarSom(HIH_F); break; //tecla 5
      case 1024: tocarSom(HIH_A); break; //tecla 6
      case 2:    tocarSom(RIDE);  break; //tecla 7
      case 32:   tocarSom(CR_1);  break; //tecla 8
      case 512:  tocarSom(CR_2);  break; //tecla 9
      case 1:   if (bpm > 0) bpm--;      //tecla *
                else bpm = 1;
                bitClr(flag, LCD);
                break;
      case 16:  bitFlp(flag, METRON);    //tecla 0
                break;
      case 256: if (bpm < 300) bpm++;    //tecla #
                else bpm = 300;
                bitClr(flag, LCD);
                break;
      default: break;
    }
  }
}