/****************************************************************************************

  O VALOR DO ATRASO � APROXIMADO.

  A incializa��o do valor de repeti��o do la�o � desconsiderada no c�mputo do atraso,
  bem como a chamada e retorno da fun��o.

  Este c�digo foi aferido com o uso de um oscilosc�pio.

  Autor: Charles Borges de Lima
  Data: 06 abril de 2015
  V. 1.1

****************************************************************************************/

#include "atrasoC.h"

//-----------------------------------------------------------------------------------------
void atraso_us(uint32_t valor)									//argumento passado pelo registrador r0
{
	asm volatile (  "movw r1, #:lower16:const_us	\n\t"
					"movt r1, #:upper16:const_us	\n\t"		//r1 = fcpu/3000000 (const_us)
					"mul  r0, r0, r1				\n\t"	    //r0 = valor*(fcpu/3000000), c�lculo do n�mero de repeti��es do la�o

					"r_us:	   subs r0, r0, #1		\n\t"
					"bne  r_us						\n\t");		//2 ciclos gastos quando tomado, 1 contr�rio
}
//-----------------------------------------------------------------------------------------
 void atraso_ms(uint32_t valor)									//argumento passado pelo registrador r0
{
	asm volatile ("movw r1, #:lower16:const_ms		\n\t"
				  "movt r1, #:upper16:const_ms		\n\t"		//r1 = fcpu/3000 (const_ms)
				  "mul  r0, r0, r1					\n\t"		//r0 = valor*(fcpu/3000), c�lculo do n�mero de repeti��es do la�o

				  "r_ms:	   subs r0, r0, #1		\n\t"
				  "bne  r_ms						\n\t");

}
//-----------------------------------------------------------------------------------------
void atraso_loops(uint32_t valor)	//argumento passado pelo registrador r0
{
	asm volatile ("r_loops:	 subs r0, r0, #1 \n\t"
							"bne  r_loops	 \n\t");
}
//-----------------------------------------------------------------------------------------
