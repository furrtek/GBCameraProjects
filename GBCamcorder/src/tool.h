/*
===============================================================================
 Name        : GBCamcorder

 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#ifndef TOOL_H_
#define TOOL_H_

uint32_t sram_banks;
uint32_t rom_banks;
uint32_t mapper_type;

void about_loop();
void about_view();
void capture_loop(void);
void capture_view(void);
void menu_loop();
void menu_view();
void view_loop(void);
void view_view(void);
void sram_loop(void);
void sram_view(void);

#endif /* TOOL_H_ */
