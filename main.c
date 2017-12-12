/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.c
 * Author: Caio
 *
 * Created on 23 de Outubro de 2017, 15:06
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include "variaveis.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#define FPS 360
#define LARGURA_TELA 640
#define ALTURA_TELA 320
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *fonte = NULL;
/*
 *
 */


void error_msg(char *text){
	al_show_native_message_box(NULL,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void display(){
 ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if(evento.type == ALLEGRO_EVENT_TIMER){
            desenha=1;
        }

        else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = 1;
        }

        if(desenha && al_is_event_queue_empty(fila_eventos)) {

        al_clear_to_color(al_map_rgb(0,0,0));

        //al_draw_pixel(5, 10, al_map_rgb(255,0,0));
               // printf("\n");
            int eixox=0,eixoy=0;
            for(int l=0; l <= 2047;l++){
                unsigned char test = gfx[l];
                if(eixox==64){
                    eixox=0;
                    eixoy+=1;
                }
                if(test == 1){
                    //al_draw_pixel(eixox, eixoy, al_map_rgb(255,255,255));
                    al_draw_filled_rectangle((eixox*10)+5, (eixoy*10)+5, (eixox*10)-5, (eixoy*10)-5, al_map_rgb(255,255,255));
                }
                else{
                    //al_draw_pixel(eixox, eixoy, al_map_rgb(0,0,0));
                    al_draw_filled_rectangle((eixox*10)+5, (eixoy*10)+5, (eixox*10)-5, (eixoy*10)-5, al_map_rgb(0,0,0));
                }
                eixox+=1;
            }

        al_flip_display();

        desenha = 0;
        }
}

void ciclo(){

   /*Pegar opcode:
    No emulador, os dados são armazenados em um array onde cada posição contém 1 byte, entretanto nosso opcode é composto de 2 bytes.
    Assim, pegamos o byte da posição atual eo o byte da proxima posição e juntamos os dois para formar nosso opcode.
    Usando o Shift left 8, acrescentamos 8 posições e finalmete usamos a operação OR para obtermos nosso opcode.
    */
    opcode = memory[pc] << 8 | memory[pc + 1];
    int altura;
    //printf("\n op: %x ", opcode);

     switch(opcode & 0xF000)
     {
         case(0x0000):
             switch(opcode & 0x00FF)
             {
                 case (0x00E0): //limpa a tela
                     for(int i = 0; i < 2048; ++i){
                         gfx[i] = 0;
                     }
                     desenha=1; //Manda apagar a tela
                     pc +=2;
                 break;

                 case(0x00EE): //retorna de uma subrotina
                     --sp; //decresce 1 para pegar o endereço certo
                    pc = stack[sp];	//o pc recebe o endereço que foi armazenado.
                     pc += 2;
                 break;
             }break;

         case(0x1000): // 0x1NNN: pula para determinado endereço NNN
             pc = opcode & 0x0FFF;
         break;

         case(0x2000): // 0x2NNN: chama subrotina em NNN.
             stack[sp] = pc; //Armazena o pc atual
             ++sp; //incremeta para caso for chamar outra subrotina
             pc = opcode & 0x0FFF;// chama a subrotina NNN
         break;

         case(0x3000): // 0x3XNN: pula a proxima instrução se VX == NN
             if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){ // d = n / 2^m
		pc += 4;
             }else{
		pc += 2;}
         break;

         case(0x4000): // 0x4XNN: pula a proxima instrução se VX != NN
             if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
		pc += 4;
             }else{
		pc += 2;}
         break;

         case(0x5000): // 0x5XY0: pula a proxima instrução se VX == VY
             if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
		pc += 4;
             }else{
		pc += 2;}
         break;

         case(0x6000): // 0x6XNN: VX == NN.
             V[(opcode & 0x0F00) >> 8] = (opcode & 0x0FF);
             pc +=2;
         break;

         case(0x7000): // 0x7XNN: VX = NN + VX.
             V[(opcode & 0x0F00) >> 8] += (opcode & 0x0FF);
             pc +=2;
         break;

         case(0x8000):
             switch(opcode & 0x000F){
                 case (0x0000): // 0x8XY0: VX = VY
                     V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                     pc +=2;
                 break;

                 case (0x0001): // 0x8XY1: VX = VX|VY
                     V[(opcode & 0x0F00) >> 8] =  V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
                     pc +=2;
                 break;

                 case (0x0002): // 0x8XY2: Vx=Vx&Vy
                     V[(opcode & 0x0F00) >> 8] =  V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
                     pc +=2;
                 break;

                 case (0x0003):  // 0x8XY3: Vx=Vx^Vy   ^ = XOR
                     V[(opcode & 0x0F00) >> 8] =  V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
                     pc +=2;
                 break;

                 case (0x0004): // 0x8XY4: Vx += Vy
                     if(V[(opcode & 0x0F00) >> 8] > (0xFF - V[(opcode & 0x00F0) >> 4 ])){
                         V[15]=1; //carry flag para caso a soma for mair que 0xFF
                     }
                     else{
                         V[15]=0;
                     }
                     V[(opcode & 0x0F00) >> 8] =  V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
                     pc +=2;
                 break;

                 case (0x0005): // 0x8XY5: Vx -= Vy
                     if(V[(opcode & 0x00F0) >> 4] > (V[(opcode & 0x0F00) >> 8 ])){
                         V[15]=0; //seta para 0, pois o numero que está subtraindo é maior que Vx
                     }
                     else{
                         V[15]=1;
                     }
                     V[(opcode & 0x0F00) >> 8] =  V[(opcode & 0x0F00) >> 8] - V[(opcode & 0x00F0) >> 4];
                     pc +=2;
                 break;

                 case (0x0006):  // 0x8XY6: Vx=Vy>>1 e V[0xF] recebe o bit menos significativo de Vx
                     V[15] = V[(opcode & 0x0F00) >> 8] & 0x1;
		     V[(opcode & 0x0F00) >> 8] >>= 1;
		     pc += 2;
                 break;

                 case (0x0007):  // 0x8XY7: Vx=Vy-Vx
                    // V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
						V[0xF] = 0;
					else
						V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					pc += 2;
                 break;

                 case (0x000E):  // 0x8XYE: Vx=Vy<<1 e V[0xF] recebe o bit mais significativo de Vx
                     V[15] = V[(opcode & 0x0F00) >> 8] >> 7;
		     V[(opcode & 0x0F00) >> 8] <<= 1;
		     pc += 2;
                 break;

                 default:printf("tem erro aí"); break;
             }
         break;

         case(0x9000): // 0x9XY0: pula proxima instrução se Vx != Vy
             if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4] ){
                 pc += 4;
             }
             else{
                 pc +=2;
             }
         break;

         case (0xA000) : // ANNN: I = NNN
             I = opcode & 0x0FFF;
             pc += 2;
	 break;

         case 0xB000: // BNNN: PC = V0+NNN / pula para o endereço NNN+V0
             pc = (opcode & 0x0FFF) + V[0];
	 break;

         case 0xC000: // CXNN: Vx=rand()&NN / VX = numero aleatorio & NN
             V[(opcode & 0x0F00) >> 8] = (rand()% 0xFF) & (opcode & 0x00FF); //rand() % (max_number + 1 - minimum_number) + minimum_number - va vdd não importa pois o resultado sempre será 1 byte
             pc +=2;
	 break;

         case 0xD000: // DXYN: Desenha um sprite na coordenada (VX, VY)
            altura = (opcode & 0x000F);
            unsigned short pixel;
            unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];

			V[0xF] = 0;
            for (int yline = 0; yline < altura; yline++) // N será a altura do sprite
			{
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++) //a largura do sprite é sempre 8
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
						if(gfx[(x + xline + ((y + yline) * 64))] == 1)
						{
							V[0xF] = 1;
						}
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

            desenha = 1;
			pc += 2;
	 break;

         case 0xE000:
            switch(opcode & 0x00FF)
            {
                case 0x009E: // EX9E: pula a proxima instrução se a key armazenada em Vx for pressionada
                    if(V[(opcode & 0x0F00) >> 8] == tecla )
                        {
						pc += 4;
                    }
					else{
						pc += 2;
					}
					//tecla=NULL;
		break;

		case 0x00A1: // EXA1: pula a proxima instrução se a key armazenada em Vx não for pressionada
                    if(V[(opcode & 0x0F00) >> 8] != tecla )
                        {
						pc += 4;
                    }
					else{
						pc += 2;
					}
					//tecla=NULL;
		break;

		default:
                    printf ("tem erro ai2");
		}
         break;

         case(0xF000):
            switch(opcode & 0x00FF)
            {
                case(0x0007):  // FX07: Vx = get_delay()
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
                break;

                case(0x000A): // FX0A:	Vx = get_key()
                    if(tecla!=0){
                        V[(opcode & 0x0F00) >> 8] = tecla;
                        pc += 2;
                    }
                    tecla=NULL;
                break;

                case(0x0015): // FX15:	delay_timer(Vx)
                    delay_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
                break;

                case(0x0018): // FX18: sound_timer(Vx)
                    sound_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
                break;

                case(0x001E):  // FX1E: I +=Vx
                    if(V[(opcode & 0x0F00)>>8] + I > 0xFFF) //caso o resultado for maior que 0xFFF pois o tamanho maximo de I é 0xFFF
                    {
                         V[15]=1; //carry flag
                    }
                    else{
                         V[15]=0;
                    }

                    I += V[(opcode & 0x0F00)>>8];
                    pc +=2;
                break;

                case(0x0029): // FX29:	I=sprite_addr[Vx]
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
                break;

                case(0x0033): // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
                    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                break;

                case(0x0055): // FX55: reg_dump(Vx,&I) =  a memória recebe valores de V0 á Vx, começando do endereço I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i){
			memory[I + i] = V[i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                break;

                case(0x0065): // FX65: 	reg_load(Vx,&I) =  V0 á Vx recebem valores da memória, começando do endereço I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i){
			V[i] = memory[I + i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                break;
                default: printf("LUL");
            }
         break;
         default: printf("LUL");
     }

	// Update timers
	if(delay_timer > 0){
		--delay_timer;
	}

	if(sound_timer > 0)
	{
		if(sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	}
}

void init(){
  pc     = 0x200;
  opcode = 0;
  I      = 0;
  sp     = 0;
    desenha = 1;
    //quando o loop principal deve encerrar
    sair = 0;
    //posicao do quadrado e quanto ele andara a cada disparo do timer, para coordenada X e Y
    for(int i = 0; i < 2048; ++i){
		gfx[i] = 0;
    }

    for(int i = 0; i < 16; ++i){
		stack[i] = 0;
    }

    for(int i = 0; i < 4096; ++i){
		memory[i] = 0;
    }

    for(int j=0;j < 80;j++){
        memory[j]=chip8_fontset[j];
    }

    if (!inicializar()){
        return -1;
    }
}

void load(){
    FILE *arq;
    int result,i;
    uint8_t ac;
   // Abre um jogo para leitura, verifique s o caminho está correto
  arq = fopen("C:\\Users\\Caio\\Desktop\\CHIP8_TheFinal\\chip8\\Games\\TETRIS", "rb");
  if (arq == NULL)  // Se houve erro na abertura
  {
     printf("Problemas na abertura do arquivo\n");
     return;
  }
  result = fread (&memory[512], sizeof(uint8_t), 4096, arq);
  printf("Nro de elementos lidos: %d\n", result);

  for (i=512; i<= (result+512); i++){
    //  printf("%d: %x\n", i, memory[i]);
  }
  printf("--LOADED--");
  fclose(arq);
}

int inicializar()
{
     if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }

    //cria o timer com o intervalo de tempo que ele ira disparar
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        error_msg("Falha ao criar temporizador");
        return 0;
    }

     if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
        return false;
    }


    if (!al_install_keyboard()){
        error_msg("Falha ao inicializar o teclado");
        return 0;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if(!janela) {
        error_msg("Falha ao criar janela");
        al_destroy_timer(timer);
        return 0;
    }

    al_set_window_title(janela, "CHIP-8 Emulador by Caio");

    al_clear_to_color(al_map_rgb(255, 0, 0));
    al_set_target_bitmap(al_get_backbuffer(janela));

    fila_eventos = al_create_event_queue();
    if(!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    return 1;
}

void teclado(){
ALLEGRO_EVENT evento;

al_wait_for_event(fila_eventos, &evento);
if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(evento.keyboard.keycode)
                {
                case ALLEGRO_KEY_1:
                    tecla = 0x1;
                    break;
                case ALLEGRO_KEY_2:
                    tecla = 0x2;
                    break;
                case ALLEGRO_KEY_3:
                    tecla = 0x3;
                    break;
                case ALLEGRO_KEY_Q:
                    tecla = 0x4;
                    break;
                case ALLEGRO_KEY_W:
                    tecla = 0x5;
                    break;
                case ALLEGRO_KEY_E:
                    tecla = 0x6;
                    break;
                case ALLEGRO_KEY_A:
                    tecla = 0x7;
                    break;
                case ALLEGRO_KEY_S:
                    tecla = 0x8;
                    break;
                case ALLEGRO_KEY_D:
                    tecla = 0x9;
                    break;
                case ALLEGRO_KEY_Z:
                    tecla = 0xA;
                    break;
                case ALLEGRO_KEY_C:
                    tecla = 0xB;
                    break;
                case ALLEGRO_KEY_4:
                    tecla = 0xC;
                    break;
                case ALLEGRO_KEY_R:
                    tecla = 0xD;
                    break;
                case ALLEGRO_KEY_F:
                    tecla = 0xE;
                    break;
                case ALLEGRO_KEY_V:
                    tecla = 0xF;
                    break;
                 case ALLEGRO_KEY_X:
                    tecla = 0x0;
                    break;
               // default: tecla = tecla;break;
                }
            }
            else if (evento.type == ALLEGRO_EVENT_KEY_UP)
            {
                tecla = NULL;
            }
}



int main(int argc, char** argv) {
    init();
    load();


    for(;;){
        teclado();
        ciclo();
        display();
    }

    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;

}
