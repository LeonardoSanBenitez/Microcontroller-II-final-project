/*
 * atrasoC.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: jeffe
 */

#ifndef ATRASOC_H_
#define ATRASOC_H_


#include <stdint.h>

asm (".equ fcpu, 72000000/2 \n\t");				//define a frequencia para o uso em c�digo assembly
asm (".equ const_ms, fcpu/3000		 \n\t");	//constante para o atraso_ms
asm (".equ const_us, fcpu/3000000	 \n\t");	//constante para o atraso_us

/*	Colocar o argumento igual a ZERO resultar� no m�ximo atraso poss�vel para a fun��o.
	N�o existe teste contra zero, visto que � um valor il�gico para a fun��o	*/


//Valores m�ximos considerando uma m�xima frequencia de trabalho de 84 MHz
//Valores fora da faixa permitida gerar�o valores irregulares de atrasos.

//VALOR M�XIMO DE 153s

void atraso_us(uint32_t valor);		//valor m�ximo de 153.000.000
void atraso_ms(uint32_t valor);		//valor m�ximo de 153.000
void atraso_loops(uint32_t valor);  //valor <= 4.294.967.296  (2^32) // repeti��o de 3 ciclos de clock por unidade de valor


#endif /* ATRASOC_H_ */
