#include "main.h"
#include "palettes.h"

//	Y = 0.257*R + 0.504*G + 0.098*B + 16
//	U = -0.148*R - 0.291*G + 0.439*B + 128
//	V = 0.439*R - 0.368*G - 0.071*B + 128
#define RGBR(in) ((in >> 16) & 0xFF)
#define RGBG(in) ((in >> 8) & 0xFF)
#define RGBB(in) (in & 0xFF)
#define RGB2YUV(v) (0.257*RGBR(v) + 0.504*RGBG(v) + 0.098*RGBB(v) + 16), \
							(-0.148*RGBR(v) - 0.291*RGBG(v) + 0.439*RGBB(v) + 128), \
							(0.439*RGBR(v) - 0.368*RGBG(v) - 0.071*RGBB(v) + 128)

const palette_t palettes[8] = {
	{{
		{ RGB2YUV(0x000000) },
		{ RGB2YUV(0x555555) },
		{ RGB2YUV(0xAAAAAA) },
		{ RGB2YUV(0xFFFFFF) }
	}, "PURE GREY"},
	{{
		{ RGB2YUV(0x394A6D) },
		{ RGB2YUV(0x3C9D9B) },
		{ RGB2YUV(0x52DE97) },
		{ RGB2YUV(0xC0FFB3) }
	}, "DMG GREEN"},
	{{
		{ RGB2YUV(0x6A2C70) },
		{ RGB2YUV(0xB83B5E) },
		{ RGB2YUV(0xF08A5D) },
		{ RGB2YUV(0xF9ED69) }
	}, "CANDY"},
	{{
		{ RGB2YUV(0x334257) },
		{ RGB2YUV(0x476072) },
		{ RGB2YUV(0x548CA8) },
		{ RGB2YUV(0xEEEEEE) }
	}, "GLACIER"},
	{{
		{ RGB2YUV(0x85603F) },
		{ RGB2YUV(0x9E7540) },
		{ RGB2YUV(0xBD9354) },
		{ RGB2YUV(0xE3D18A) }
	}, "PAPER"},
	{{
		{ RGB2YUV(0x000000) },
		{ RGB2YUV(0x3E432E) },
		{ RGB2YUV(0x616F39) },
		{ RGB2YUV(0xA7D129) }
	}, "PINE"},
	{{
		{ RGB2YUV(0x3A0088) },
		{ RGB2YUV(0x930077) },
		{ RGB2YUV(0xE61C5D) },
		{ RGB2YUV(0xFFBD39) }
	}, "DUSK"},
	{{
		{ RGB2YUV(0xFFFFFF) },
		{ RGB2YUV(0xAAAAAA) },
		{ RGB2YUV(0x555555) },
		{ RGB2YUV(0x000000) }
	}, "INVERTED"}
};

