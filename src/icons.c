/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/
#include "LPC13xx.h"

const uint8_t logo[2+16+(89*30)] = {
	178, 30,
	0x67, 0x66, 0x45, 0x24, 0x23, 0x02, 0x46, 0x00, 0x01, 0x4A, 0xFF, 0x94, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0x45, 0x55, 0x43, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x57, 0x77, 0x77, 0x77, 0x77, 0x77, 0x86, 0x08, 0x88, 0x88, 0x88, 0x88, 0x88, 0x53, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x13, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x72, 0x17, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x87, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x71, 0x67, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x47, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x70, 0x27, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x18, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x80, 0x47, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x10, 0x00, 0x00, 0x01, 0x63, 0x45, 0x55, 0x55, 0x55, 0x42, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x55, 0x53, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x87, 0x77, 0x77, 0x77, 0x77, 0x53, 0x26, 0x11, 0x16, 0x34, 0x40, 0x57, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x30, 0x00, 0x00, 0x24, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x47, 0x77, 0x77, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x05, 0x77, 0x77, 0x77, 0x77, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x77, 0x78, 0x00, 0x00, 0x12, 0x77, 0x77, 0x77, 0x40, 0x00, 0x03, 0x77, 0x77, 0x79, 0x99, 0x99, 0x99, 0x97, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x99, 0x97, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x37, 0x77, 0x77, 0x77, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x77, 0x77, 0x75, 0x00, 0x00, 0x00, 0x47, 0x77, 0x77, 0x40, 0x00, 0x37, 0x77, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xA7, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x79, 0xAA, 0xA7, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x77, 0x77, 0x77, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x77, 0x77, 0x73, 0x00, 0x00, 0x00, 0x37, 0x77, 0x77, 0x40, 0x02, 0x77, 0x7B, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x79, 0xAA, 0xA7, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x77, 0x77, 0x77, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x77, 0x77, 0x72, 0x00, 0x00, 0x00, 0x57, 0x77, 0x77, 0x20, 0x15, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x74, 0x33, 0x33, 0x33, 0x32, 0x60, 0x01, 0x23, 0x33, 0x33, 0x33, 0x26, 0x62, 0x33, 0x33, 0x21, 0x00, 0x00, 0x00, 0x16, 0x23, 0x33, 0x33, 0x32, 0x10, 0x16, 0x23, 0x33, 0x33, 0x33, 0x21, 0x00, 0x06, 0x23, 0x33, 0x32, 0x61, 0x16, 0x23, 0x33, 0x47, 0x7B, 0xAA, 0xB7, 0x72, 0x01, 0x62, 0x33, 0x32, 0x61, 0x00, 0x00, 0x62, 0x33, 0x33, 0x26, 0x00,
	0x07, 0x77, 0x77, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x76, 0x00, 0x00, 0x03, 0x77, 0x77, 0x78, 0x00, 0x27, 0x7B, 0xAA, 0xAA, 0xB9, 0x99, 0x99, 0x99, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x83, 0x24, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x78, 0x21, 0x00, 0x63, 0x57, 0x77, 0x77, 0x77, 0x77, 0x53, 0x57, 0x77, 0x77, 0x77, 0x77, 0x75, 0x26, 0x28, 0x77, 0x77, 0x77, 0x84, 0x57, 0x77, 0x77, 0x77, 0x7B, 0xAA, 0xB7, 0x73, 0x35, 0x77, 0x77, 0x77, 0x74, 0x20, 0x12, 0x87, 0x77, 0x77, 0x78, 0x20,
	0x67, 0x77, 0x77, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x77, 0x77, 0x75, 0x44, 0x44, 0x87, 0x77, 0x77, 0x72, 0x00, 0x37, 0x7A, 0xAA, 0xA9, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x73, 0x62, 0x87, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x78, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x7A, 0xAA, 0x97, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x73, 0x47, 0x77, 0x77, 0x77, 0x77, 0x86,
	0x27, 0x77, 0x77, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x40, 0x01, 0x57, 0x9A, 0xAA, 0xA7, 0x77, 0x77, 0x77, 0x77, 0x77, 0xBA, 0xAA, 0xAA, 0xAA, 0xAB, 0x97, 0x77, 0x9A, 0xAA, 0xAA, 0xAA, 0xB7, 0x9B, 0xAA, 0xAA, 0xB7, 0x77, 0x57, 0x77, 0x9B, 0xBA, 0xAA, 0xAA, 0xA9, 0x77, 0x7B, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x77, 0x77, 0xBA, 0xAA, 0xAB, 0x77, 0x7B, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x97, 0x77, 0x79, 0xBB, 0xAA, 0xAB, 0x97, 0x77, 0x77, 0x7B, 0xAA, 0xAA, 0xB7, 0x72,
	0x47, 0x77, 0x77, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x78, 0x00, 0x06, 0x87, 0x9A, 0xAA, 0xB7, 0x77, 0x77, 0x77, 0x77, 0x77, 0xBA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA7, 0x79, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0x77, 0x7B, 0xAA, 0xAA, 0xAA, 0xAA, 0xA7, 0x79, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB, 0x77, 0x9A, 0xAA, 0xAA, 0xAB, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0x79, 0xBA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0x79, 0xAA, 0xAA, 0xAA, 0xB7, 0x72,
	0x57, 0x77, 0x75, 0x00, 0x00, 0x00, 0x24, 0x44, 0x44, 0x44, 0x44, 0x40, 0x17, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x20, 0x06, 0x77, 0xBA, 0xAA, 0x97, 0x77, 0x77, 0x77, 0x77, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA9, 0x79, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA7, 0x7A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0xBA, 0xAA, 0xAA, 0xAB, 0x7B, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x7B, 0xAA, 0xAA, 0xAA, 0xB7, 0x76,
	0x87, 0x77, 0x74, 0x00, 0x00, 0x00, 0x57, 0x77, 0x77, 0x77, 0x77, 0x70, 0x27, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x71, 0x02, 0x77, 0xBA, 0xAA, 0x97, 0x77, 0x77, 0x77, 0x77, 0x77, 0x99, 0x99, 0x99, 0x99, 0xAA, 0xA9, 0x7B, 0xAA, 0xA7, 0x77, 0x7A, 0xAA, 0xA9, 0x77, 0x7A, 0xAA, 0xA7, 0x7B, 0xAA, 0xAA, 0xB9, 0x99, 0x99, 0x97, 0x9A, 0xAA, 0xB9, 0x99, 0x99, 0xAA, 0xAA, 0x97, 0xAA, 0xAA, 0x99, 0x97, 0x9A, 0xAA, 0xAB, 0x99, 0x99, 0xAA, 0xAB, 0x77, 0xAA, 0xAA, 0x97, 0x77, 0x9A, 0xAA, 0xB7, 0x7A, 0xAA, 0xA9, 0x99, 0x77, 0x51,
	0x87, 0x77, 0x75, 0x00, 0x00, 0x00, 0x57, 0x77, 0x77, 0x77, 0x77, 0x50, 0x37, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x75, 0x03, 0x77, 0xAA, 0xAA, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xAA, 0xA9, 0x7B, 0xAA, 0xB7, 0x77, 0x7B, 0xAA, 0xB7, 0x77, 0x79, 0xAA, 0xA7, 0x7A, 0xAA, 0xB7, 0x77, 0x77, 0x77, 0x77, 0xBA, 0xAB, 0x77, 0x77, 0x77, 0x9A, 0xAA, 0x79, 0xAA, 0xA9, 0x77, 0x77, 0xBA, 0xAA, 0x77, 0x77, 0x77, 0xBA, 0xAB, 0x7B, 0xAA, 0xB7, 0x77, 0x77, 0x79, 0xAA, 0xB7, 0x9A, 0xAA, 0x97, 0x77, 0x77, 0x20,
	0x87, 0x77, 0x78, 0x00, 0x00, 0x00, 0x87, 0x77, 0x77, 0x77, 0x77, 0x40, 0x47, 0x77, 0x77, 0x61, 0x11, 0x11, 0x65, 0x77, 0x77, 0x77, 0x64, 0x77, 0xAA, 0xAA, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x7B, 0xBB, 0xBB, 0xBB, 0xAA, 0xA7, 0x7A, 0xAA, 0x97, 0x77, 0x7A, 0xAA, 0xB7, 0x77, 0x7B, 0xAA, 0xB7, 0x9A, 0xAA, 0x77, 0x77, 0x77, 0x77, 0x77, 0xBA, 0xAB, 0x77, 0x77, 0x77, 0x9A, 0xAA, 0x7B, 0xAA, 0xA7, 0x77, 0x77, 0xAA, 0xA9, 0x77, 0x77, 0x77, 0xAA, 0xA9, 0x7A, 0xAA, 0xBB, 0xBB, 0xBB, 0xBB, 0xAA, 0xB7, 0xBA, 0xAA, 0x77, 0x75, 0x42, 0x00,
	0x47, 0x77, 0x77, 0x60, 0x00, 0x01, 0x77, 0x77, 0x77, 0x77, 0x77, 0x30, 0x57, 0x77, 0x77, 0x00, 0x00, 0x00, 0x00, 0x87, 0x77, 0x77, 0x35, 0x79, 0xAA, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x77, 0x79, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA7, 0x7A, 0xAA, 0x97, 0x77, 0x7A, 0xAA, 0x97, 0x77, 0x7B, 0xAA, 0xB7, 0x9A, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x77, 0xAA, 0xA9, 0x77, 0x77, 0x77, 0xBA, 0xAB, 0x7B, 0xAA, 0xA7, 0x77, 0x79, 0xAA, 0xA7, 0x77, 0x77, 0x77, 0xAA, 0xA7, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0xBA, 0xAA, 0x77, 0x31, 0x00, 0x00,
	0x37, 0x77, 0x77, 0x40, 0x00, 0x06, 0x44, 0x44, 0x77, 0x77, 0x77, 0x20, 0x77, 0x77, 0x78, 0x00, 0x00, 0x00, 0x00, 0x37, 0x77, 0x77, 0x47, 0x7B, 0xAA, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x77, 0x7A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x9A, 0xAA, 0x77, 0x77, 0x9A, 0xAA, 0x77, 0x77, 0x7A, 0xAA, 0xB7, 0xBA, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x77, 0xAA, 0xA7, 0x77, 0x77, 0x77, 0xBA, 0xAB, 0x7A, 0xAA, 0xB7, 0x77, 0x7B, 0xAA, 0xB7, 0x77, 0x77, 0x79, 0xAA, 0xA7, 0xBA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x97, 0xAA, 0xAB, 0x77, 0x20, 0x00, 0x00,
	0x67, 0x77, 0x77, 0x76, 0x00, 0x00, 0x00, 0x00, 0x77, 0x77, 0x77, 0x61, 0x77, 0x77, 0x74, 0x00, 0x00, 0x00, 0x00, 0x47, 0x77, 0x77, 0x37, 0x7B, 0xAA, 0xA9, 0x77, 0x77, 0x77, 0x77, 0x77, 0x9A, 0xAA, 0xBB, 0xBB, 0xBA, 0xAA, 0xB7, 0xBA, 0xAA, 0x77, 0x77, 0x9A, 0xAA, 0x77, 0x77, 0x7A, 0xAA, 0x97, 0xBA, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x79, 0xAA, 0xA7, 0x77, 0x77, 0x77, 0xAA, 0xA9, 0x7A, 0xAA, 0xB7, 0x77, 0x7B, 0xAA, 0xB7, 0x77, 0x77, 0x7B, 0xAA, 0xB7, 0xBA, 0xAA, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0x77, 0xAA, 0xAB, 0x77, 0x60, 0x00, 0x00,
	0x08, 0x77, 0x77, 0x77, 0x20, 0x00, 0x00, 0x01, 0x77, 0x77, 0x77, 0x06, 0x77, 0x77, 0x72, 0x00, 0x00, 0x00, 0x02, 0x77, 0x77, 0x77, 0x27, 0x7B, 0xAA, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x77, 0xBA, 0xA9, 0x77, 0x77, 0x7A, 0xAA, 0xB7, 0xBA, 0xAB, 0x77, 0x77, 0xBA, 0xAB, 0x77, 0x77, 0x9A, 0xAA, 0x77, 0xBA, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x7B, 0xAA, 0xA7, 0x77, 0x77, 0x79, 0xAA, 0xA9, 0x9A, 0xAA, 0x97, 0x77, 0x7B, 0xAA, 0xA7, 0x77, 0x77, 0x7B, 0xAA, 0xB7, 0xBA, 0xAB, 0x77, 0x77, 0x77, 0x77, 0x77, 0x79, 0xAA, 0xA9, 0x78, 0x60, 0x00, 0x00,
	0x02, 0x77, 0x77, 0x77, 0x74, 0x21, 0x00, 0x14, 0x77, 0x77, 0x78, 0x02, 0x77, 0x77, 0x73, 0x66, 0x66, 0x62, 0x47, 0x77, 0x77, 0x75, 0x67, 0x7B, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB, 0xBB, 0x77, 0xAA, 0xAB, 0x99, 0x99, 0x9A, 0xAA, 0x97, 0xBA, 0xAB, 0x77, 0x77, 0xBA, 0xAB, 0x77, 0x77, 0x9A, 0xAA, 0x77, 0xBA, 0xAA, 0xB9, 0x99, 0x99, 0x97, 0x7B, 0xAA, 0xAB, 0x99, 0x99, 0xBA, 0xAA, 0xA7, 0x9A, 0xAA, 0x97, 0x77, 0x7B, 0xAA, 0xAA, 0xB9, 0x99, 0x9A, 0xAA, 0xB7, 0xBA, 0xAA, 0xB9, 0x99, 0x99, 0x99, 0x77, 0x79, 0xAA, 0xA9, 0x75, 0x10, 0x00, 0x00,
	0x00, 0x47, 0x77, 0x77, 0x77, 0x77, 0x88, 0x77, 0x77, 0x77, 0x74, 0x04, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x76, 0x15, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0xAA, 0xA9, 0x77, 0x77, 0xAA, 0xA9, 0x77, 0x77, 0xBA, 0xAB, 0x77, 0x7A, 0xAA, 0xAA, 0xAA, 0xAA, 0xA9, 0x79, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0xBA, 0xAA, 0x77, 0x77, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x97, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x7B, 0xAA, 0xA7, 0x73, 0x00, 0x00, 0x00,
	0x00, 0x05, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x72, 0x08, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x30, 0x02, 0x77, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0xBA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0xAA, 0xA7, 0x77, 0x77, 0xAA, 0xA9, 0x77, 0x77, 0xBA, 0xAB, 0x77, 0x79, 0xAA, 0xAA, 0xAA, 0xAA, 0xA7, 0x77, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0xBA, 0xAA, 0x77, 0x77, 0x77, 0xBA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0x7B, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x97, 0x7B, 0xAA, 0xA7, 0x72, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x47, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x70, 0x07, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x73, 0x00, 0x01, 0x47, 0x79, 0xBA, 0xAA, 0xAA, 0xAA, 0xAB, 0x77, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB, 0x79, 0xAA, 0xA7, 0x77, 0x79, 0xAA, 0xA7, 0x77, 0x77, 0xAA, 0xA9, 0x77, 0x77, 0x9B, 0xAA, 0xAA, 0xAA, 0xA7, 0x77, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xB7, 0x77, 0xAA, 0xAB, 0x77, 0x34, 0x87, 0x7B, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x77, 0x77, 0x9B, 0xBA, 0xAA, 0xAA, 0xAA, 0x97, 0x7A, 0xAA, 0xB7, 0x72, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x20, 0x17, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x20, 0x00, 0x00, 0x65, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x75, 0x77, 0x77, 0x77, 0x74, 0x77, 0x77, 0x77, 0x75, 0x57, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x75, 0x60, 0x28, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x51, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x13, 0x57, 0x77, 0x77, 0x77, 0x75, 0x31, 0x00, 0x67, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x75, 0x31, 0x00, 0x00, 0x00, 0x01, 0x38, 0x77, 0x77, 0x77, 0x77, 0x77, 0x75, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x36, 0x47, 0x77, 0x77, 0x46, 0x37, 0x77, 0x77, 0x46, 0x13, 0x87, 0x77, 0x77, 0x77, 0x77, 0x44, 0x77, 0x77, 0x77, 0x77, 0x77, 0x75, 0x47, 0x77, 0x77, 0x52, 0x00, 0x06, 0x57, 0x77, 0x77, 0x77, 0x77, 0x77, 0x73, 0x63, 0x87, 0x77, 0x77, 0x77, 0x77, 0x78, 0x77, 0x77, 0x75, 0x20, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x33, 0x22, 0x10, 0x00, 0x00, 0x06, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x23, 0x33, 0x33, 0x33, 0x32, 0x61, 0x23, 0x33, 0x33, 0x33, 0x33, 0x32, 0x22, 0x33, 0x32, 0x10, 0x12, 0x33, 0x32, 0x10, 0x16, 0x33, 0x32, 0x10, 0x00, 0x62, 0x33, 0x33, 0x33, 0x32, 0x10, 0x62, 0x33, 0x33, 0x33, 0x32, 0x61, 0x16, 0x33, 0x32, 0x60, 0x00, 0x00, 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x61, 0x00, 0x12, 0x23, 0x33, 0x33, 0x33, 0x26, 0x23, 0x33, 0x26, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t logo_fe[2+16+(45*25)] = {
	90, 25,
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x01, 0x11, 0x11, 0x10, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x11, 0x00, 0x00, 0x11, 0x11, 0x11, 0x00, 0x01, 0x11, 0x11, 0x10, 0x11, 0x00, 0x11, 0x00, 0x11, 0x10, 0x01, 0x11, 0x01, 0x11, 0x11, 0x00, 0x11, 0x11, 0x10, 0x01, 0x10, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x11, 0x10, 0x00, 0x01, 0x11, 0x11, 0x10, 0x11, 0x00, 0x11, 0x01, 0x11, 0x10, 0x11, 0x11, 0x01, 0x11, 0x11, 0x01, 0x10, 0x00, 0x11, 0x01, 0x10, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x00, 0x11, 0x00, 0x11, 0x01, 0x10, 0x00, 0x11, 0x00, 0x00, 0x11, 0x00, 0x01, 0x11, 0x11, 0x11, 0x01, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00, 0x11, 0x11, 0x00, 0x01, 0x11, 0x00, 0x01, 0x10, 0x00, 0x00, 0x11, 0x00, 0x11, 0x01, 0x10, 0x00, 0x11, 0x00, 0x00, 0x11, 0x00, 0x01, 0x10, 0x00, 0x00, 0x01, 0x10, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x00, 0x01, 0x11, 0x11, 0x10, 0x00, 0x11, 0x00, 0x01, 0x10, 0x00, 0x00, 0x11, 0x11, 0x11, 0x01, 0x10, 0x00, 0x11, 0x00, 0x00, 0x11, 0x11, 0x01, 0x11, 0x11, 0x11, 0x01, 0x10, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x11, 0x11, 0x00, 0x01, 0x10, 0x01, 0x10, 0x00, 0x11, 0x00, 0x01, 0x10, 0x00, 0x00, 0x01, 0x11, 0x11, 0x01, 0x10, 0x00, 0x11, 0x00, 0x00, 0x01, 0x11, 0x00, 0x11, 0x11, 0x11, 0x01, 0x10, 0x00, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x11, 0x11, 0x00, 0x01, 0x10, 0x01, 0x10, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x00, 0x01, 0x11, 0x11, 0x10, 0x01, 0x11, 0x00, 0x00, 0x11, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00, 0x11, 0x11, 0x00, 0x01, 0x10, 0x00, 0x01, 0x11, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00, 0x01, 0x11, 0x10, 0x11, 0x10, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00, 0x00, 0x11, 0x11, 0x11, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x11, 0x00, 0x00, 0x11, 0x10, 0x00, 0x00, 0x01, 0x11, 0x11, 0x10, 0x11, 0x11, 0x10, 0x00, 0x11, 0x11, 0x10, 0x11, 0x01, 0x11, 0x11, 0x00, 0x01, 0x11, 0x11, 0x00, 0x01, 0x11, 0x11, 0x00, 0x01, 0x11, 0x10, 0x11, 0x01, 0x11, 0x11, 0x00, 0x01, 0x11, 0x11,
	0x00, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 0x10, 0x11, 0x11, 0x11, 0x01, 0x11, 0x11, 0x10, 0x11, 0x01, 0x11, 0x11, 0x10, 0x11, 0x00, 0x01, 0x10, 0x11, 0x00, 0x01, 0x10, 0x11, 0x11, 0x10, 0x11, 0x01, 0x11, 0x11, 0x10, 0x11, 0x11, 0x11,
	0x00, 0x01, 0x10, 0x01, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x11, 0x00, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x11, 0x10, 0x11, 0x00, 0x00, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x00, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x11, 0x00, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x00, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11, 0x10, 0x11, 0x00, 0x11, 0x01, 0x11, 0x11, 0x10, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x11, 0x10, 0x11, 0x00, 0x00, 0x11, 0x01, 0x10, 0x01, 0x10, 0x11, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x10, 0x11, 0x00, 0x11, 0x00, 0x11, 0x11, 0x10, 0x11, 0x01, 0x10, 0x01, 0x10, 0x01, 0x11, 0x11, 0x10, 0x01, 0x11, 0x11, 0x10, 0x11, 0x00, 0x00, 0x11, 0x01, 0x10, 0x01, 0x10, 0x01, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x10,
};

const uint8_t icon_sdok[2+16+(8*16)] = {
	10, 16,
	0x18, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x01, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x01,
	0x11, 0x11, 0x11, 0x11, 0x11,
	0x12, 0x22, 0x12, 0x22, 0x11,
	0x21, 0x11, 0x12, 0x11, 0x21,
	0x12, 0x21, 0x12, 0x11, 0x21,
	0x11, 0x12, 0x12, 0x11, 0x21,
	0x22, 0x21, 0x12, 0x22, 0x11,
};

const uint8_t icon_sdnok[2+16+(5*16)] = {
	10, 16,
	0x18, 0x67, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x01, 0x11,
	0x00, 0x00, 0x00, 0x00, 0x11,
	0x22, 0x00, 0x00, 0x22, 0x01,
	0x02, 0x20, 0x02, 0x20, 0x01,
	0x00, 0x22, 0x22, 0x00, 0x01,
	0x00, 0x02, 0x20, 0x00, 0x01,
	0x00, 0x22, 0x22, 0x00, 0x01,
	0x02, 0x20, 0x02, 0x20, 0x01,
	0x22, 0x00, 0x00, 0x22, 0x01,
	0x11, 0x11, 0x11, 0x11, 0x11,
	0x12, 0x22, 0x12, 0x22, 0x11,
	0x21, 0x11, 0x12, 0x11, 0x21,
	0x12, 0x21, 0x12, 0x11, 0x21,
	0x11, 0x12, 0x12, 0x11, 0x21,
	0x22, 0x21, 0x12, 0x22, 0x11,
};

const uint8_t icon_camok[2+16+(8*16)] = {
	16, 16,
	0x67, 0xDE, 0x94, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x12, 0x21, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x12, 0x22, 0x22, 0x21, 0x00, 0x00,
	0x00, 0x01, 0x22, 0x13, 0x31, 0x22, 0x10, 0x00,
	0x00, 0x02, 0x23, 0x33, 0x33, 0x32, 0x20, 0x00,
	0x00, 0x12, 0x31, 0x11, 0x11, 0x13, 0x21, 0x00,
	0x00, 0x22, 0x21, 0x14, 0x41, 0x11, 0x22, 0x00,
	0x00, 0x22, 0x22, 0x14, 0x41, 0x11, 0x22, 0x00,
	0x00, 0x22, 0x11, 0x11, 0x11, 0x11, 0x22, 0x00,
	0x00, 0x02, 0x21, 0x11, 0x11, 0x12, 0x20, 0x00,
	0x00, 0x02, 0x22, 0x11, 0x11, 0x22, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x44, 0x00, 0x44, 0x00, 0x40, 0x00, 0x40,
	0x04, 0x00, 0x04, 0x00, 0x40, 0x44, 0x04, 0x40,
	0x04, 0x00, 0x04, 0x44, 0x40, 0x40, 0x40, 0x40,
	0x04, 0x00, 0x04, 0x00, 0x40, 0x40, 0x00, 0x40,
	0x00, 0x44, 0x04, 0x00, 0x40, 0x40, 0x00, 0x40,
};

const uint8_t icon_camnok[2+16+(8*16)] = {
	16, 16,
	0x67, 0xDE, 0x94, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x12, 0x21, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x12, 0x22, 0x22, 0x21, 0x00, 0x00,
	0x00, 0x01, 0x22, 0x13, 0x31, 0x22, 0x10, 0x00,
	0x00, 0x02, 0x44, 0x33, 0x33, 0x44, 0x20, 0x00,
	0x00, 0x12, 0x34, 0x41, 0x14, 0x43, 0x21, 0x00,
	0x00, 0x22, 0x21, 0x44, 0x44, 0x11, 0x22, 0x00,
	0x00, 0x22, 0x22, 0x14, 0x41, 0x11, 0x22, 0x00,
	0x00, 0x22, 0x11, 0x44, 0x44, 0x11, 0x22, 0x00,
	0x00, 0x02, 0x24, 0x41, 0x14, 0x42, 0x20, 0x00,
	0x00, 0x02, 0x44, 0x11, 0x11, 0x44, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x44, 0x00, 0x44, 0x00, 0x40, 0x00, 0x40,
	0x04, 0x00, 0x04, 0x00, 0x40, 0x44, 0x04, 0x40,
	0x04, 0x00, 0x04, 0x44, 0x40, 0x40, 0x40, 0x40,
	0x04, 0x00, 0x04, 0x00, 0x40, 0x40, 0x00, 0x40,
	0x00, 0x44, 0x04, 0x00, 0x40, 0x40, 0x00, 0x40,
};
