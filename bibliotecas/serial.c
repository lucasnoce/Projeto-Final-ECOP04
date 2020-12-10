// -----------------------------------------------------------------------
//   Copyright (C) Rodrigo Almeida 2010
// -----------------------------------------------------------------------
//   Arquivo: serial.c
//            Biblioteca da porta serial (USART) do PIC18F4520
//   Autor:   Rodrigo Maximiano Antunes de Almeida
//            rodrigomax at unifei.edu.br
//   Licen�a: GNU GPL 2
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

#include "serial.h"
#include <pic18f4520.h>
#include "io.h"
#include "bits.h"

void serialSend(unsigned char c) {
    while (!bitTst(PIR1, 4)); //aguarda o registro ficar disponivel
    TXREG = c; //coloca o valor para ser enviado
}

void serialInit(void) {
    TXSTA = 0b00101100; //configura a transmissao de dados da serial
    RCSTA = 0b10010000; //configura a recepcao de dados da serial
    BAUDCON = 0b00001000; //configura sistema de velocidade da serial
    SPBRGH = 0; //configura para 56k
    SPBRG = 34; //configura para 56k
    bitSet(TRISC, 6); //pino de recepcao de dados
    bitSet(TRISC, 7); //pino de envio de dados
}