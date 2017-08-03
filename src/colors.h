#include "main.h"

#define COLOR_RED 		0b1111100000000000
#define COLOR_GREEN 	0b0000011111100000
#define COLOR_BLUE		0b0000000000011111
#define COLOR_YELLOW	0b1111111111100000
#define COLOR_ORANGE	0b1111101111100000
#define COLOR_CYAN		0b0000011111111111
#define COLOR_WHITE 	0b1111111111111111
#define COLOR_GREY		0b0111101111101111
#define COLOR_BLACK		0b0000000000000000

const uint16_t lut_2bpp_grey[4];
const uint16_t lut_2bpp_dmg[4];
const uint16_t lut_2bpp_cgb[4];
const uint16_t * lut_2bpp_list[3];

const uint16_t bar_colors[8];
