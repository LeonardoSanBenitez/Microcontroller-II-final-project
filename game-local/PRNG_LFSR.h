#ifndef PRNG_LFSR_H_
#define PRNG_LFSR_H_

#include <stdio.h>

unsigned long lfsr;

//--------------------------------------------------------------------------------
/*
 * Função que passa o valor "semente" para a variavel utilizada na função prng_LFSR()
 */
void init_LFSR(unsigned long valor)
{
	lfsr = valor;
}
//--------------------------------------------------------------------------------
/*
 * Função PRNG - toda vez que for chamada, um novo número é gerado e salvo em lfsr.
 * 	O primeiro valor de lfsr deve ser diferente de zero e deve ser gerado como semente
 */
unsigned long  prng_LFSR() 	// Galois LFSRs, Liner-Feedback Shift Register, (PRNG)
{
	unsigned long lsb;

	if (lfsr==0)		// garantia para que valor nao seja zero
	    lfsr = 1;

	lsb = lfsr & 0x00000001;
	lfsr = lfsr >> 1;

	if (lsb)
	    lfsr = lfsr ^ 0x80000057;	//polinomio retirado de http://users.ece.cmu.edu/~koopman/lfsr/

	return lfsr;
}
//--------------------------------------------------------------------------------




#endif /* PRNG_LFSR_H_ */
