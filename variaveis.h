/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   variaveis.h
 * Author: Caio
 *
 * Created on 23 de Outubro de 2017, 18:18
 */

#ifndef VARIAVEIS_H
#define VARIAVEIS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

//0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
//0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
//0x200-0xFFF - Program ROM and work RAM

unsigned short opcode;
unsigned char memory[4096]; //O chip 8 tem uma memória de 4K
unsigned char V[16]; //Registradores que vão de V0 á VE, o 16º registrador é usado como "carry flag"
unsigned short I; //Index register - pode ter valores de 0x000 á 0xFFF
unsigned short pc; //program counter - pode ter valores de 0x000 á 0xFFF
int desenha=0,sair=0; // Desenha ou não

unsigned short stack[16]; //Toda vez que é realizado um jump ou chamada de subrotina, deve-se armazenar o pc no stack
unsigned short sp; // Stack pointer - em q nivel do stack o pc está armazenado

unsigned char gfx[64 * 32];//será util no futuro
//int gfx[64 * 32];

unsigned char  delay_timer;		// Delay timer
unsigned char  sound_timer;		// Sound timer

unsigned char delay_timer; //delay do timer
unsigned char sound_timer; //delay do som

#endif /* VARIAVEIS_H */

