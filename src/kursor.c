#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h> 

#include "tgaReader.h"
#include "bits.h"

typedef struct {
    uint8_t Width;
    uint8_t Height;
    uint64_t PixelMapOffset;
    uint64_t BitmapMaskOffset;
} KursorHeader;

int main() {

    FILE* tgaFile = fopen("img/test.tga", "rb");

	fseek(tgaFile, 0, SEEK_END);
    int size = ftell(tgaFile);
	fseek(tgaFile, 0, SEEK_SET);

    uint8_t* buffer = tgaMalloc(size);
    fread(buffer, size, 1, tgaFile);

    uint8_t* pixel = tgaRead(buffer, TGA_READER_ARGB);
    uint8_t tgaBtpp = tgaGetDepth(buffer) / 8;

    uint64_t width = tgaGetWidth(buffer);
    uint64_t height = tgaGetHeight(buffer);
    uint64_t pitch = tgaGetWidth(buffer) * 4;

    uint64_t HeaderSize = sizeof(KursorHeader);
    uint64_t PixelMapSize = width * pitch;
    uint64_t BitmapSize = ceilf(width * pitch / 8);

    uint64_t KursorSize = HeaderSize + PixelMapSize + BitmapSize;
    uintptr_t Kursor = malloc(KursorSize);

    uint8_t* PixelMap = (uintptr_t) Kursor + HeaderSize;
    uint8_t* BitmapMask = (uintptr_t) Kursor + (HeaderSize + PixelMapSize);

    for(uint64_t h = 0; h < height; h++) {
        for(uint64_t w = 0; w < width; w++) {
            uint64_t tgaPitch = w * tgaBtpp; 

            if(pixel[w * tgaBtpp + h * tgaPitch] == 0)
                BIT_CLEAR(BitmapMask[h * w / 8], h * w % 8);
            else
                BIT_SET(BitmapMask[h * w / 8], h * w % 8);

            for(uint64_t btpp = 1; btpp < 4; btpp++) {
                PixelMap[w * 3 + h * (w * 3) + btpp] = pixel[w * tgaBtpp + h * tgaPitch + btpp];
            }
        }
    }

    fclose(tgaFile);
    
    KursorHeader header = { .Width = width, .Height = height, .PixelMapOffset = HeaderSize, .BitmapMaskOffset = HeaderSize + PixelMapSize};

    FILE* file = fopen("default.kursor", "w");

    fwrite(Kursor, KursorSize, 1, file);

    fclose(file);
    
    return NULL;
}