#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h> 

#include "tgaReader.h"
#include "bits.h"

#define KURSOR_VERSION  "0"
#define KURSOR_REVISION 1

typedef struct {
    uint8_t Revision;
    uint8_t Width;
    uint8_t Height;
    uint64_t PixelMapOffset;
    uint64_t BitmapMaskOffset;
} __attribute__((__packed__)) KursorHeader;

int main(int argc, char* argv[]) {

    if(argc < 3) {
        printf("TGA to Kursor converter\n");
        printf("Version: %s\n\n", KURSOR_VERSION);
        printf("Usage: ./convert [InputFile].tga [OutputFile].kursor\n");

        return EXIT_FAILURE;
    }

    FILE* tgaFile = fopen(argv[1], "rb");

    if(tgaFile == NULL) {
        printf("The file doesn't exist.\n");

        return EXIT_FAILURE;
    }

	fseek(tgaFile, 0, SEEK_END);
    uint64_t size = ftell(tgaFile);
	fseek(tgaFile, 0, SEEK_SET);

    uint64_t* buffer = (uint64_t*) tgaMalloc(size);
    fread(buffer, size, 1, tgaFile);

    uint32_t* pixels = (uint32_t*) tgaRead(buffer, TGA_READER_ARGB);
    uint8_t tgaBtpp = tgaGetDepth(buffer) / 8;

    uint32_t width = tgaGetWidth(buffer);
    uint32_t height = tgaGetHeight(buffer);
    uint32_t pitch = width * 4;

    tgaFree(buffer);

    uint64_t HeaderSize = sizeof(KursorHeader);
    uint64_t PixelMapSize = height * pitch;
    uint64_t BitmapSize = ceilf(height * pitch / 8);

    uint64_t KursorSize = HeaderSize + PixelMapSize + BitmapSize;
    uintptr_t Kursor = malloc(KursorSize);

    uint32_t* PixelMap = (uintptr_t) Kursor + HeaderSize;
    uint8_t* BitmapMask = (uintptr_t) Kursor + (HeaderSize + PixelMapSize);
    
    int x = 0;
    for(uint32_t h = 0; h < height; h++) {
        for(uint32_t w = 0; w < width; w++) {
            uint32_t tgaPitch = w * tgaBtpp; 

            if((pixels[x] >> 24) & 0xFF == 0)
                BIT_CLEAR(BitmapMask[x / 8], x % 8);
            else
                BIT_SET(BitmapMask[x / 8], x % 8);

            PixelMap[x] = pixels[x];

            x++;
        }
    }

    fclose(tgaFile);

    KursorHeader* Header = (KursorHeader*) Kursor;

    Header->Revision = KURSOR_REVISION;
    Header->Width = width;
    Header->Height = height;
    Header->PixelMapOffset = HeaderSize;
    Header->BitmapMaskOffset = HeaderSize + PixelMapSize;

    FILE* file = fopen(argv[2], "w");

    fwrite(Kursor, KursorSize, 1, file);

    free(Kursor);

    fclose(file);
    
    return EXIT_SUCCESS;
}