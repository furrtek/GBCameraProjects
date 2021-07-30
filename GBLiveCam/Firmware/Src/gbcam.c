#include "main.h"
#include "gbcam.h"

// Dithering layout taken from real GB Cam (see routine at ROMA:4427)
// Table of matrix offsets * 3, order is top-bottom, left-right
//     0   1   2   3
//   ---------------
// 0 | A   M   D   P
// 1 | I   E   L   H
// 2 | C   O   B   N
// 3 | K   G   J   F
static const uint8_t matrix_layout[16] = { 0, 30, 24, 6,
										15, 45, 39, 21,
										12, 42, 36, 18,
										3, 33, 27, 9 };

// Contrast values for computing dithering matrix
uint8_t qlevels[4];

/* Sensor signal, full range
 * |              /
 * |            /
 * |          /
 * |        /
 * |      /
 * |    /
 * |  /
 * |/_______________
 *     |  |  |  |    qlevels
 *
 * The lower qlevels the higher the brightness (goes into small signal)
 * The most closely spaced qlevels the higher the contrast
 *
 * Sensor signal, under-exposed
 * |
 * |
 * |
 * |
 * |
 * |    /
 * |  /
 * |/_______________
 *   ||||            qlevels
 *
 * Sensor signal, over-exposed
 * |              /
 * |            /
 * |          /
 * |
 * |
 * |
 * |
 * |________________
 *             ||||  qlevels
 *
 */

static void wait_short() {
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

void gbcam_init() {
	gbcam_reset();
    gbcam_setmatrix(10, 100);	// Basic values for interior lighting

    // Clear GB Cam scratchpad RAM (bank 0, A000~AFFF)
    cart_put(0x4000, 0x00);		// SRAM bank 0
    cart_put(0x0000, 0x0A);		// Enable SRAM writes
    for (uint32_t c = 0xA000; c < 0xB000; c++)
    	cart_put(c, 0x00);

    gbcam_setgain(5);
    cart_put(0xA004, 0x45);		// Edge enhance 100%, Vref=0.0V
    //cart_put(0xA005, 0x3F | 0x00 | 0x00);		// Max level calibration, useless ?
}

uint8_t gbcam_wait_busy() {
	uint32_t timeout = GBC_TIMEOUT;

	cart_put(0x4000, 0x10);		// ASIC registers
	while (!(cart_get_ram(0xA000) & 1)) {
		if (timeout)
			timeout--;
		else
			return 1;
	}
	return 0;
}

uint8_t gbcam_wait_idle() {
	uint32_t timeout = GBC_TIMEOUT;

	cart_put(0x4000, 0x10);		// ASIC registers
	while (cart_get_ram(0xA000) & 1) {
		if (timeout)
			timeout--;
		else
			return 1;
	}
	return 0;
}

void cart_set_address(const uint16_t address) {
	ALEL_HIGH
	ALEH_HIGH
	GBCCS_HIGH
	GBCRD_HIGH
	GBCWR_HIGH

	GBBUS_OUTPUT

	// Set address low
	GBBUS_SET(address & 0x00FF)
	wait_short();
	ALEL_LOW

	// Set address high
	GBBUS_SET(address >> 8)
	wait_short();
	ALEH_LOW
}

uint8_t cart_get_ram(const uint16_t address) {
	uint8_t v;

	cart_set_address(address);

	GBBUS_INPUT

	GBCCS_LOW
	wait_short();
	GBCRD_LOW

	delay_us(1);

	v = GBBUS_READ

	GBCRD_HIGH
	GBCCS_HIGH

	return v;
}

uint8_t cart_get_rom(const uint16_t address) {
	cart_set_address(address);

	GBBUS_INPUT

	GBCCS_HIGH
	GBCRD_LOW

	delay_us(1);

	uint8_t v = GBBUS_READ

	GBCRD_HIGH

	return v;
}

void cart_put(const uint16_t address, const uint8_t value) {
	cart_set_address(address);
	delay_us(1);

	GBBUS_OUTPUT
	GBBUS_SET(value)

	wait_short();

	if (address >= 0xA000) {
		GBCCS_LOW
		delay_us(1);
	}

	GBCWR_LOW
	delay_us(3);				// TODO: Make shorter ?
	GBCWR_HIGH

	if (address >= 0xA000) {
		delay_us(1);
		GBCCS_HIGH
	}
}

// Returns 0 if GB Cam is detected
uint32_t gbcam_detect() {
	uint32_t c, result = 0;
	char buffer[13];

	// Game Boy Camera (USA, Europe):		"GAMEBOYCAMERA"
	// Game Boy Camera Gold (USA):			"GAMEBOYCAMERA G"
	// Pocket Camera (Japan) (Rev A):		"POCKETCAMERA"
	// Pocket Camera (J) (V1.1) [S] (1):	"POCKETCAMERA"
	const char name_jp[14] = "POCKETCAMERA\x0";
	const char name_ex[14] = "GAMEBOYCAMERA";

	// Disable Phi generation and set it low
	HAL_TIM_OC_Stop(&s_Timer12, TIM_CHANNEL_1);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);

	cart_put(0x0000, 0x0A);			// Initialize MBC, allow writing to RAM

	// Load ID string from ROM bank 0
	for (c = 0; c < 13; c++)
		buffer[c] = cart_get_rom(0x0134 + c);

	// Check if it matches any of the correct IDs
	for (c = 0; c < 13; c++) {
		if ((buffer[c] != name_jp[c]) && (buffer[c] != name_ex[c])) {
			result = 1;
			break;
		}
	}

	// Re-enable Phi generation
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_ALTERNATE);
	HAL_TIM_OC_Start(&s_Timer12, TIM_CHANNEL_1);

	// If previous test passed, check if A000 bit 0 is = 0 (idle)
	if (!result)
		result = gbcam_wait_idle();

	return result;
}

void gbcam_reset() {
	GBCRST_LOW
	delay_us(20000);
	GBCRST_HIGH
	delay_us(10000);
}

// Compute GB Cam dithering matrix values (see GB Cam ROM)
void gbcam_setmatrix(const uint8_t slope, const uint8_t offset) {
	uint8_t gbcam_matrix[48];	// Dithering ADC matrix
	uint32_t c;
	uint16_t acc, inc, v;
	uint8_t pixel;

	// This is a hardcoded 16 entry LUT @ ROMA:7C20 in the GB Cam ROM
	// Slope ~= -contrast, offset ~= -brightness
    for (c = 0; c < 4; c++) {
    	v = offset + (c * slope);
    	qlevels[c] = (v < 256) ? v : 255;
    }

	// Each entry in the 4x4 matrix is 3 bytes indicating the threshold levels for each shade of grey
	// Pixel voltage < byte A -> black
	// byte A < Pv < byte B -> dark grey
	// byte B < Pv < byte C -> light grey
	// byte C < Pv -> white
	// Those are used to generate the voltages for the 3 comparators in the GB Cam ASIC
	// The GB Cam ROM generates the whole matrix from 4 bytes: qlevels array (and threshold levels also),
	// which are "spread out" by interpolation on the 16 pixels.
	// Bytes A for each pixel are qlevels[0] -> qlevels[1]
	// Bytes B for each pixel are qlevels[1] -> qlevels[2]
	// Bytes C for each pixel are qlevels[2] -> qlevels[3]
    for (c = 0; c < 3; c++) {
    	acc = qlevels[c];
    	inc = qlevels[c + 1] - acc;	// Delta between 2 qlevels
    	acc <<= 4;					// 4.4 fixed point
		for (pixel = 0; pixel < 16; pixel++) {
			gbcam_matrix[c + matrix_layout[pixel]] = acc >> 4;
			acc += inc;
		}
    }

    cart_put(0x4000, 0x10);		// ASIC registers
	delay_us(10);
	gbcam_wait_idle();

	for (c = 0; c < 48; c++)
		cart_put(0xA006 + c, gbcam_matrix[c]);
}

void gbcam_setgain(const uint32_t gain) {
	cart_put(0x4000, 0x10);		// ASIC registers
	delay_us(100);
	gbcam_wait_idle();

	cart_put(0xA001, gain & 15);
}

void gbcam_setexposure(const uint16_t exposure) {
	cart_put(0x4000, 0x10);		// ASIC registers
	delay_us(100);
	gbcam_wait_idle();

	cart_put(0xA002, exposure >> 8);
	cart_put(0xA003, exposure & 0xFF);
}
