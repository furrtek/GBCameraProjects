#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned char bmp_header[54] = {
	'B', 'M',					// Magic
	0x76, 0x1C, 0x00, 0x00,		// Size of file
	0x00, 0x00, 0x00, 0x00,		// Reserved
	0x76, 0x00, 0x00, 0x00,		// Offset to image data

	0x28, 0x00, 0x00, 0x00,		// Header size
	0x80, 0x00, 0x00, 0x00,		// Image width
	0x70, 0x00, 0x00, 0x00,		// Image height
	0x01, 0x00,					// Planes
	0x04, 0x00,					// Bits per pixel
	0x00, 0x00, 0x00, 0x00,		// Compression
	0x00, 0x1C, 0x00, 0x00,		// Image data size
	0x00, 0x00, 0x00, 0x00,		// X resolution
	0x00, 0x00, 0x00, 0x00,		// Y resolution
	0x00, 0x00, 0x00, 0x00,		// Colors count
	0x00, 0x00, 0x00, 0x00		// Important colors
};

const unsigned char bmp_palette[64] = {
	0x20, 0x20, 0x20, 0x00,
	0x6A, 0x6A, 0x6A, 0x00,
	0xB4, 0xB4, 0xB4, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF,
	
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

unsigned char bmp_frame[7168];

void write_bmp(const unsigned short number) {
	FILE * file_out;
	char filename[13] = { 0 };
	
	sprintf(filename, "OUT%05d.BMP", number);
	
	file_out = fopen(filename, "wb+");
	fwrite(bmp_header, 1, sizeof(bmp_header), file_out);
	fwrite(bmp_palette, 1, sizeof(bmp_palette), file_out);
	fwrite(bmp_frame, sizeof(unsigned long), 14336, file_out);
	fclose(file_out);
}

int main(int argc, char *argv[]) {
	FILE * file_in;
	FILE * file_audio_out;
	long file_size, pos;
	unsigned short frame_number, skipped, x, y, xp, yto, plane_pos;
	unsigned char * buffer_in, * frame_ptr;
	unsigned char pixel, prev_pixel;
	
	if (argc != 2) {
		puts("Usage: converter input.bin");
		return 1;
	}
	
	file_in = fopen(argv[1], "rb");
	
	fseek(file_in, 0, SEEK_END);
	file_size = ftell(file_in);
	buffer_in = malloc(file_size);
	
	if (buffer_in == NULL) {
		puts("Memory allocation failed");
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
	
	file_audio_out = fopen("OUT.SND", "wb+");
	
	pos = 16;
	do {
		if (buffer_in[pos] == 'V') {
			// Frame
			skipped = buffer_in[pos+1] + 1;
			// if (skipped > 5) puts("Skip error");
			while (skipped) {
				// Duplicate last frame as needed
				//write_bmp(frame_number++);
				skipped--;
			}
			// Decode new frame
			frame_ptr = &buffer_in[pos+2];
            for (y = 0; y < 112; y++) {
                yto = (y & 7) + ((y & 0x78) << 4);
                for (x = 0; x < 128; x++) {
                	plane_pos = ((x & 0x78) + yto) << 1;
                    xp = 7 - (x & 7);
                    pixel = ((frame_ptr[plane_pos] >> xp) & 1) | (((frame_ptr[plane_pos + 1] >> xp) << 1) & 2);
                    
                    if (x & 1)
						bmp_frame[(((111 - y) << 7) + x) >> 1] = (prev_pixel << 4) | pixel;
					
                    prev_pixel = pixel;
                }
			}
			pos += (3584 + 2);
		} else if (buffer_in[pos] == 'A') {
			// Audio
			skipped = buffer_in[pos+1];
			pos += 2;
			
			while (skipped) {
				fwrite(&buffer_in[pos], 1, 512, file_audio_out);
				pos += 512;
				skipped--;
			}
			
		} else {
			pos++;
		}
	} while (pos < file_size);
	
	fclose(file_audio_out);
	free(buffer_in);
	
	return 0;
}

