// Converter for GameBoy Camcorder video files
// 2018 furrtek
// CC Attribution-NonCommercial-ShareAlike 4.0
// https://github.com/furrtek/GBCamcorder

typedef struct {
	unsigned short left;
	unsigned short top;
	unsigned short right;
	unsigned short bottom;
} RECT;

typedef struct {
	unsigned char fcc[4];
	unsigned long cb;
	unsigned long dwMicroSecPerFrame;
	unsigned long dwMaxBytesPerSec;
	unsigned long dwPaddingGranularity;
	unsigned long dwFlags;
	unsigned long dwTotalFrames;
	unsigned long dwInitialFrames;
	unsigned long dwStreams;
	unsigned long dwSuggestedBufferSize;
	unsigned long dwWidth;
	unsigned long dwHeight;
	unsigned long dwReserved[4];
} AVIMAINHEADER;

typedef struct {
	unsigned char fccType[4];
	unsigned char fccHandler[4];
	unsigned long dwFlags;
	unsigned short wPriority;
	unsigned short wLanguage;
	unsigned long dwInitialFrames;
	unsigned long dwScale;
	unsigned long dwRate;
	unsigned long dwStart;
	unsigned long dwLength;
	unsigned long dwSuggestedBufferSize;
	unsigned long dwQuality;
	unsigned long dwSampleSize;
	RECT rcFrame;
} AVISTREAMHEADER;

typedef struct {
	unsigned long biSize;
	unsigned long biWidth;
	unsigned long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	unsigned long biXPelsPerMeter;
	unsigned long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
	unsigned long start;
	unsigned long size;
	void * parent;
} chunk_t;

AVIMAINHEADER avi_main_header;
AVISTREAMHEADER avi_video_header;
AVISTREAMHEADER avi_audio_header;
BITMAPINFOHEADER bitmap_info_header;

void resizechunk(chunk_t * chunk, unsigned long size);
chunk_t startchunk(FILE * file, chunk_t * parent, char * type, char * fourcc);
void endchunk(FILE * file, chunk_t * chunk);
void fillchunk(FILE * file, chunk_t * chunk, void * data, unsigned long size);

