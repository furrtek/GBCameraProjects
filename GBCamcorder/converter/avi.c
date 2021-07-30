// Converter for GameBoy Camcorder video files
// 2018 furrtek
// CC Attribution-NonCommercial-ShareAlike 4.0
// https://github.com/furrtek/GBCamcorder

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avi.h"

AVIMAINHEADER avi_main_header = {
	{ 'a', 'v', 'i', 'h' },
	14 * 4,
	1000000/16,
	(128*112/2)*16 + 8192,
	0,
	0,		// To check
	0,		// dwTotalFrames
	0,
	2,
	0,
	128,
	112,
	{ 0, 0, 0, 0 }
};

AVISTREAMHEADER avi_video_header = {
	{ 'v', 'i', 'd', 's' },
	{ 'D', 'I', 'B', ' ' },
	0,
	0,
	0,
	0,
	1,
	16,
	0,
	0,	// dwLength
	32768,
	10000,
	0,
	{ 0, 0, 128, 112 }
};

AVISTREAMHEADER avi_audio_header = {
	{ 'a', 'u', 'd', 's' },
	{ 'P', 'C', 'M', ' ' },
	0,
	1,
	0,
	0,
	1,
	8192,
	0,
	0,	// dwLength
	32768,
	10000,
	0,
	{ 0, 0, 0, 0 }
};

BITMAPINFOHEADER bitmap_info_header = {
	40,
	128,
	112,
	1,
	24,
	0,
	128*112*3,
	0,
	0,
	0,
	0
};

void resizechunk(chunk_t * chunk, unsigned long size) {
	chunk_t * ptr = chunk;
	
	while (ptr != NULL) {
		ptr->size += size;
		ptr = ptr->parent;
	}
}

chunk_t startchunk(FILE * file, chunk_t * parent, char * type, char * fourcc) {
	long int start = ftell(file);
	fwrite(type, 1, 4, file);
	fseek(file, 4, SEEK_CUR);
	if (fourcc != NULL) {
		fwrite(fourcc, 1, 4, file);
		resizechunk(parent, 12);
		chunk_t r = { start, 4, parent };
		return r;
	} else {
		resizechunk(parent, 8);
		chunk_t r = { start, 0, parent };
		return r;
	}
}

void endchunk(FILE * file, chunk_t * chunk) {
	long int ref = ftell(file);
	fseek(file, chunk->start + 4, SEEK_SET);
	fwrite(&chunk->size, 1, 4, file);
	fseek(file, ref, SEEK_SET);
}

void fillchunk(FILE * file, chunk_t * chunk, void * data, unsigned long size) {
	resizechunk(chunk, size);
	fwrite(data, 1, size, file);
}

