// -----------------------------------------------------------------------
//   Copyright (C) Rodrigo Almeida 2014
// -----------------------------------------------------------------------
//   Arquivo: keypad.c
//            Biblioteca de opera??o de um teclado matricial
//   Autor:   Rodrigo Maximiano Antunes de Almeida
//            rodrigomax at unifei.edu.br
//   Licen?a: GNU GPL 2
// -----------------------------------------------------------------------
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; version 2 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// -----------------------------------------------------------------------

#include "keypad.h"
#include <pic18f4520.h>
#include "io.h"
#include "bits.h"

static unsigned char tempo;
static unsigned int valorNovo = 0x0000;
static unsigned int valorAntigo = 0x0000;
static unsigned int valor = 0x0000;

unsigned int kpRead (void){
  return valor;
}

void kpDebounce (void){
  unsigned char i, j;
  
  //store D to avoid mess with other periphels
  unsigned char old_portD = PORTD;
  unsigned char old_portB = PORTB;
  
  unsigned char old_trisD = TRISD;
  unsigned char old_trisB = TRISB;

  //PORTD ? compartilhado, ent?o tem
  //que garantir que ? entrada
  TRISD |= 0x0f;

  for (i = 0; i < 3; i++){
    //desabilita todas as colunas
    TRISB &= 0xF8;
    //Habilita apenas a coluna desejada
    bitSet(TRISB, i);
    //coloca nivel alto s? na coluna desejada
    PORTB |= 0x07;
    bitClr(PORTB, i);

    //delay pra estabilizar os sinais
    for (int k = 0; k < 5; k++);

    //realiza o teste para cada bit e atualiza a vari?vel
    for (j = 0; j < 4; j++){
      if (!bitTst(PORTD, j)){
        bitSet(valorNovo, (i * 4) + j);
      }
      else{
        bitClr(valorNovo, (i * 4) + j);
      }
    }
  }
  
  //retorna as portas ao estado anterior ao debounce
  PORTD = old_portD;
  PORTB = old_portB;
  TRISD = old_trisD;
  TRISB = old_trisB;
  
  if (valorAntigo == valorNovo){
    tempo--;
  }
  else{
    tempo = 2;
    valorAntigo = valorNovo;
  }
  if (tempo == 0){
    valor = valorAntigo;
  }
  else{
    valor = 0x0000;
  }
}

void kpInit (void){
  //coloca 0-2 como sa?da
  TRISB &= 0x07;
  //coloca 0-3 como entrada
  TRISD |= 0x0f;
}