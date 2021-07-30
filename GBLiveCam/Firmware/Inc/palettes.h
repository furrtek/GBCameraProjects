#ifndef __PALETTES_H
#define __PALETTES_H

typedef struct {
	uint8_t Y, U, V;
} YUV_t;

typedef struct {
	YUV_t color[4];
	char name[11];
} palette_t;

extern const palette_t palettes[8];

#endif /* __PALETTES_H */
