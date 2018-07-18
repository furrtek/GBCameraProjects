// Converter for GameBoy Camcorder video files
// 2018 furrtek
// CC Attribution-NonCommercial-ShareAlike 4.0
// https://github.com/furrtek/GBCamcorder

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avi.h"

typedef struct {
	unsigned char pixels[128*112];
	unsigned short repeat;
} frame_t;

frame_t * frames;

unsigned char dib_frame[128*112*3];

typedef struct {
	unsigned char R, G, B;
} RGB;

const RGB rgb_lut[4] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x55, 0x55, 0x55 },
	{ 0xAA, 0xAA, 0xAA },
	{ 0xFF, 0xFF, 0xFF }
};

int main(int argc, char *argv[]) {
	FILE * file_in;
	FILE * file_out;
	//FILE * file_audio_out;
	long file_size, pos, dib_index;
	unsigned short frame_number = 0, skipped, x, y, xp, yto, plane_pos;
	unsigned char * buffer_in, * frame_ptr;
	unsigned char pixel;
	
	if (argc != 2) {
		puts("Usage: converter file.bin");
		return 1;
	}
	
	file_in = fopen(argv[1], "rb");
	
	fseek(file_in, 0, SEEK_END);
	file_size = ftell(file_in);
	buffer_in = malloc(file_size);
	
	if (buffer_in == NULL) {
		puts("Memory allocation 1 failed");
		return 1;
	}
	
	// Testing
	frames = malloc(sizeof(frame_t) * 200);
	if (frames == NULL) {
		puts("Memory allocation 2 failed");
		return 1;
	}
	
	rewind(file_in);
	fread(buffer_in, 1, file_size, file_in);
	fclose(file_in);
	
	if (strncmp((char*)buffer_in, "GBCC", 4)) {
		puts("Wrong file type");
		return 1;
	}
	
	puts("File loaded OK");
	
	//file_audio_out = fopen("OUT.SND", "wb+");
	
	pos = 16;
	do {
		if (buffer_in[pos] == 'V') {
			// Frame
			skipped = buffer_in[pos+1];
			// if (skipped > 5) puts("Skip error");
			if (frame_number) {
				frames[frame_number-1].repeat = skipped;
			}
			// Decode new frame
			frame_ptr = &buffer_in[pos+2];
            for (y = 0; y < 112; y++) {
                yto = (y & 7) + ((y & 0x78) << 4);
                for (x = 0; x < 128; x++) {
                	plane_pos = ((x & 0x78) + yto) << 1;
                    xp = 7 - (x & 7);
                    pixel = ((frame_ptr[plane_pos] >> xp) & 1) | (((frame_ptr[plane_pos + 1] >> xp) << 1) & 2);
                    
                    frames[frame_number].pixels[((111 - y) << 7) + x] = pixel;
                }
			}
			frame_number++;
			pos += (3584 + 2);
		} else if (buffer_in[pos] == 'A') {
			// Audio
			skipped = buffer_in[pos+1];
			pos += 2;
			
			while (skipped) {
				//fwrite(&buffer_in[pos], 1, 512, file_audio_out);
				pos += 512;
				skipped--;
			}
			
		} else {
			pos++;
		}
	} while (pos < file_size);
	
	//fclose(file_audio_out);
	free(buffer_in);
	
	
	file_out = fopen("out.avi", "wb+");
	
	chunk_t chunk_avi = startchunk(file_out, NULL, "RIFF", "AVI ");
	
	chunk_t chunk_hdrl = startchunk(file_out, &chunk_avi, "LIST", "hdrl");
	fillchunk(file_out, &chunk_hdrl, &avi_main_header, sizeof(avi_main_header));
	
	chunk_t chunk_strl = startchunk(file_out, &chunk_hdrl, "LIST", "strl");
	
	chunk_t chunk_strh = startchunk(file_out, &chunk_strl, "strh", NULL);
	fillchunk(file_out, &chunk_strh, &avi_video_header, sizeof(avi_video_header));
	endchunk(file_out, &chunk_strh);
	chunk_t chunk_strf = startchunk(file_out, &chunk_strl, "strf", NULL);
	fillchunk(file_out, &chunk_strf, &bitmap_info_header, sizeof(bitmap_info_header));
	endchunk(file_out, &chunk_strf);
	
	endchunk(file_out, &chunk_strl);
	endchunk(file_out, &chunk_hdrl);

	chunk_t chunk_movi = startchunk(file_out, &chunk_avi, "LIST", "movi");
	
	for (x = 0; x < frame_number - 1; x++) {
		for (dib_index = 0; dib_index < (128*112); dib_index++) {
			pixel = frames[x].pixels[dib_index];
			dib_frame[dib_index*3] = rgb_lut[pixel].R;
			dib_frame[dib_index*3+1] = rgb_lut[pixel].G;
			dib_frame[dib_index*3+2] = rgb_lut[pixel].B;
		}
		for (y = 0; y < frames[x].repeat + 1; y++) {
			chunk_t chunk_db = startchunk(file_out, &chunk_movi, "00db", NULL);
			fillchunk(file_out, &chunk_db, &dib_frame, sizeof(dib_frame));
			endchunk(file_out, &chunk_db);
		}
	}
	
	endchunk(file_out, &chunk_movi);
	
	endchunk(file_out, &chunk_avi);
	
	fclose(file_out);
	
	free(frames);
	
	return 0;
}

