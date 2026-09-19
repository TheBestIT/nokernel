#ifndef FONT_H
#define FONT_H

#include "shared/libs/bitmap.h"
#include "kernel/include/dev/framebuffer.h"

uint32_t Size4ByteArrayToUint32(uint8_t array[4]);
uint16_t Size2ByteArrayToUint16(uint8_t array[2]);
uint8_t  Size1ByteArrayToUint8(uint8_t array[1]);

struct font_t {
    uint8_t *BMPDataSegmentOffsetted;   // The pixel data. Byte math needs a byte pointer.
    uint32_t BMPDataSegmentSize;
    uint32_t BMPImageWidth;
    uint32_t BMPImageHeight;

    uint32_t CellWidth;
    uint32_t CellHeight;

    uint16_t BitsPerPixel;
    uint32_t Stride;

    uint32_t CellsPerXAxis;
    uint32_t CellsPerYAxis;
    uint32_t TotalCells;
};

font_t buildFontStruct(Bitmap *fontBitmap, uint32_t fontCellWidth, uint32_t fontCellHeight);

#endif // FONT_H