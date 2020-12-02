#include <pic18f4520.h>
#include "bits.h"
#include "config.h"
#include "io.h"
#include "keypad.h"
#include "serial.h"
#include "keypad.h"
#include "timer.h"
#include "lcd.h"

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

void tocarSom(unsigned char key){
  serialSend(0x90); //note ON event; ch 1
  serialSend(key); //key
  serialSend(0x50); //velocity
}


void main(void){
  kpInit();
  timerInit();
  serialInit();
  lcdInit();
  
  unsigned char flag = 0xC0;
  static unsigned int bpm = 120;
  unsigned int countMetron = 0;
  print(bpm);
  
  unsigned int valorTeclado; //valor lido pelo telcado
  unsigned char valorSerial;
  float i;

  for(;;){
    if (timerEnded() && bitTst(flag, METRON)){
      
      timerReset(10000); //datasheet pic18f4520
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
    
    
    //leitura do teclado
    kpDebounce();
    valorTeclado = kpRead();
    switch (valorTeclado){
      case 1:    valorSerial = 12; break;
      case 2:    valorSerial = 7; break;
      case 4:    valorSerial = 4; break;
      case 8:    valorSerial = 1; break;
      case 16:   valorSerial = 0; break;
      case 32:   valorSerial = 8; break;
      case 64:   valorSerial = 5; break;
      case 128:  valorSerial = 2; break;
      case 256:  valorSerial = 14; break;
      case 512:  valorSerial = 9; break;
      case 1024: valorSerial = 6; break;
      case 2048: valorSerial = 3; break;
      default:   valorSerial = 10; break;
    }
    if (valorSerial != 10){
      switch (valorSerial){
        case 1: tocarSom(BUMBO); break;
        case 2: tocarSom(CAIXA); break;
        case 3: tocarSom(TOM_1); break;
        case 4: tocarSom(TOM_2); break;
        case 5: tocarSom(HIH_F); break;
        case 6: tocarSom(HIH_A); break;
        case 7: tocarSom(RIDE); break;
        case 8: tocarSom(CR_1); break;
        case 9: tocarSom(CR_2); break;
        case 12: if (bpm > 0) bpm--;
                 else bpm = 1;
                 bitClr(flag, LCD);
                 break;
        case  0: bitFlp(flag, METRON);
                 break;
        case 14: if (bpm < 300) bpm++;
                 else bpm = 300;
                 bitClr(flag, LCD);
                 break;
        default: break;
      }
    }
  }
}