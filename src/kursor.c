#include <stdio.h>
#include <stdlib.h>

#include "tgaReader.h"

typedef struct {
    u_int8_t Width;
    u_int8_t Height;
    u_int64_t PixelPosition;
    u_int64_t BitmapMaskPosition;
} Kursor;

#define HEADER_SIZE sizeof(Kursor)

int main() {

    FILE* tgaFile = fopen("img/test.tga", "rb");

	fseek(tgaFile, 0, SEEK_END);
    int size = ftell(tgaFile);
	fseek(tgaFile, 0, SEEK_SET);

    u_int8_t* buffer = tgaMalloc(size);
    fread(buffer, size, 1, tgaFile);

    int* pixel = tgaRead(buffer, TGA_READER_ABGR);
    printf("width %d: ", tgaGetWidth(buffer));

    fclose(tgaFile);

    Kursor kursor;

    Kursor header = { .Width = 10, .Height = 10, .PixelPosition = NULL, .BitmapMaskPosition = NULL};

    FILE* file = fopen("default.kursor", "w");

    fwrite(&header, HEADER_SIZE, 1, file);

    fclose(file);

    /* FILE* file2 = fopen("default.kursor", "r");

    fread(&kursor, HEADER_SIZE, 1, file2);

    printf("data: %d \n", kursor.Width);

    fclose(file2); */
    
    return NULL;
}