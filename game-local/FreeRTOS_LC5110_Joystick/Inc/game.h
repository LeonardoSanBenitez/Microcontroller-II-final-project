/*
 * game.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: jeffe
 */

#ifndef GAME_H_
#define GAME_H_


#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif


#endif /* GAME_H_ */
