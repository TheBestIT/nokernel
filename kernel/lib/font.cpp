#include "kernel/include/lib/font.h"

uint32_t Size4ByteArrayToUint32(uint8_t array[4]) {
    return    (uint32_t)array[0] 
            | ((uint32_t)array[1] << 8)
            | ((uint32_t)array[2] << 16)
            | ((uint32_t)array[3] << 24);
}

uint16_t Size2ByteArrayToUint16(uint8_t array[2]) {
    return (uint16_t)(array[0] | (array[1] << 8));
}

uint8_t Size1ByteArrayToUint8(uint8_t array[1]) {
    return array[0];
}

font_t buildFontStruct(Bitmap *fontBitmap, uint32_t fontCellWidth, uint32_t fontCellHeight) {
    font_t fontDescriptor;

    fontDescriptor.BMPDataSegmentOffsetted = (uint8_t *)fontBitmap->BaseAddress
                                                 + Size4ByteArrayToUint32(fontBitmap->Header->DataOffset);
    fontDescriptor.BMPDataSegmentSize = Size4ByteArrayToUint32(fontBitmap->Header->ImageSize);
    fontDescriptor.BMPImageWidth = Size4ByteArrayToUint32(fontBitmap->Header->Width);
    fontDescriptor.BMPImageHeight = Size4ByteArrayToUint32(fontBitmap->Header->Height);

    fontDescriptor.CellHeight = fontCellHeight;
    fontDescriptor.CellWidth  = fontCellWidth;

    fontDescriptor.BitsPerPixel = Size2ByteArrayToUint16(fontBitmap->Header->BitsPerPixel);
    // The stride is a property of the image row, not of the cell.
    fontDescriptor.Stride = ((fontDescriptor.BMPImageWidth * fontDescriptor.BitsPerPixel + 31) / 32) * 4;

    fontDescriptor.CellsPerXAxis = fontDescriptor.BMPImageWidth / fontCellWidth;
    fontDescriptor.CellsPerYAxis = fontDescriptor.BMPImageHeight / fontCellHeight;
    fontDescriptor.TotalCells    = fontDescriptor.CellsPerXAxis * fontDescriptor.CellsPerYAxis;

    return fontDescriptor;
}