#ifndef SHARED_BITMAP_H
#define SHARED_BITMAP_H

#include <stdint.h>

typedef struct BitmapHeader {
    // Header
    uint8_t Signature[2];
    uint8_t FileSize[4];
    uint8_t Reserved[4];
    uint8_t DataOffset[4];

    // InfoHeader
    uint8_t Size[4];
    uint8_t Width[4];
    uint8_t Height[4];
    uint8_t Planes[2];
    uint8_t BitsPerPixel[2];
    uint8_t Compression[4];
    uint8_t ImageSize[4];
    uint8_t XPixelsPerM[4];
    uint8_t YPixelsPerM[4];
    uint8_t ColorsUsed[4];
    uint8_t ImportantColors[4];

    // ColorTable
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t reserved;
} BitmapHeader;

typedef struct Bitmap {
    BitmapHeader *Header;
    uint64_t *BaseAddress;
} Bitmap;

#endif // SHARED_BITMAP_H