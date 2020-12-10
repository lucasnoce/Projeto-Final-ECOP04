/*
 * File:   main_projeto.c
 * Author: Lucas Arturo Noce
 *
 * Created on November 6, 2020, 11:51 PM
 */

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

unsigned char flag = 0xC0;            //variavel usada para controle
static unsigned int bpm = 120;        //andamento que pode ser redefinido pelo usuario
unsigned int countMetron = 0;         //contador do metronomo
unsigned int countMetronMax;          //valor max do contador, de acordo com o andamento
static unsigned char velocity = 0x50; //usado como medida de volume neste projeto


////////////////////////////////////////////////////////////////////////////////
//  FUNCOES                                                                   //

void tocarSom(unsigned char key){ //envia de acordo com o protocolo MIDI
  serialSend(0x90);     //note ON event no canal 1
  serialSend(key);      //key
  serialSend(velocity); //velocity
}

void checarTimer(){ //estrutura do metronomo eh baseada em um timer de 5ms
  if (bitTst(flag, METRON)){ //se o metronomo esta ligado
    countMetron++;
    if (countMetron >= countMetronMax){ //contador chegou ao valor max
      bitClr(TRISB, 7); //acende o LED
      bitSet(PORTB, 7); //
      countMetron = 0;
    }
    else if (countMetron == 20){ //apaga o LED depois de 100ms (20*5ms)
      bitClr(TRISB, 7);
      bitClr(PORTB, 7);
    }
  }
}

void checarLCD(){
  if (!bitTst(flag, METRON) && bitTst(flag, LCD)){ //imprime Metronomo OFF
    bitClr(TRISB, 7);
    bitClr(PORTB, 7);
    print(-1);
    bitClr(flag, LCD);
  }
  else if (bitTst(flag, METRON) && !bitTst(flag, LCD)){ //imprime o andamento
    print(bpm);
    bitSet(flag, LCD);
  }
}

void checarADC(){ //faz a leitura do trimpot e configura o volume
  velocity = adc_amostra() >> 3;
}


////////////////////////////////////////////////////////////////////////////////
//  PROGRAMA PRINCIPAL                                                        //
void main(void){
  kpInit();     //inicializa o teclado
  timerInit();  //inicializa o timer
  serialInit(); //inicializa a serial
  lcdInit();    //inicializa o LCD
  
  //calcula o numero max de contador com base no andamento
  countMetronMax = 60000/(5*bpm);
  
  //imprime o valor do andamento no LCD
  print(bpm);
  
  unsigned int valorTeclado; //valor lido pelo telcado
  unsigned char n = 0;       //usado no switch

  //loop
  for(;;){
    timerReset(5000); //reseta o timer para 5ms
    
    //usado para melhor eficiencia temporal
    switch (n){
      case 0: checarADC();
              n = 1;
              break;
      case 1: checarLCD();
              n = 0;
              break;
      default: n = 0; break;
    }
    
    //leitura do teclado
    kpDebounce();
    valorTeclado = kpRead();
    
    //a tecla * diminui em uma unidade o andamento do metronomo
    //a tecla # aumenta em uma unidade o andamento do metronomo
    //nessas teclas o valor max do contador eh redefinido ja que bpm foi alterado
    //a tecla 0 tem funcao liga/desliga e espera um tempo antes para evitar bouncing
    switch (valorTeclado){
      case 0:    break;                          //nenhuma tecla
      case 8:    tocarSom(BUMBO); break;         //tecla 1
      case 128:  tocarSom(CAIXA); break;         //tecla 2
      case 2048: tocarSom(TOM_1); break;         //tecla 3
      case 4:    tocarSom(TOM_2); break;         //tecla 4
      case 64:   tocarSom(HIH_F); break;         //tecla 5
      case 1024: tocarSom(HIH_A); break;         //tecla 6
      case 2:    tocarSom(RIDE);  break;         //tecla 7
      case 32:   tocarSom(CR_1);  break;         //tecla 8
      case 512:  tocarSom(CR_2);  break;         //tecla 9
      case 1:    if (bpm > 1) bpm--;             //tecla *
                 else bpm = 1;
                 bitClr(flag, LCD);
                 countMetronMax = 60000/(5*bpm);
                 break;
      case 16:   for (int i=0; i<100; i++);      //tecla 0
                 bitFlp(flag, METRON);
                 break;
      case 256:  if (bpm < 300) bpm++;           //tecla #
                 else bpm = 300;
                 bitClr(flag, LCD);
                 countMetronMax = 60000/(5*bpm);
                 break;
      default:   break;
    }
    
    timerWait();   //espera chegar aos 5ms
    checarTimer(); //checa se tem que piscar o led a cada 5ms exatamente
  }
}