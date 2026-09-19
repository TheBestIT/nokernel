#ifndef GOP_H
#define GOP_H

#include "Base.h"

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL; 

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    uint32_t            RedMask;
    uint32_t            GreenMask;
    uint32_t            BlueMask;
    uint32_t            ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
    uint32_t                    Version;
    uint32_t                    HorizontalResolution;
    uint32_t                    VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT   PixelFormat;
    EFI_PIXEL_BITMASK           PixelInformation;
    uint32_t                    PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

// 12.9.2.1: EFI_GRAPHICS_OUTPUT_PROTOCOL.QueryMode()
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
    EFI_GRAPHICS_OUTPUT_PROTOCOL         *This,
    uint32_t                             ModeNumber,
    UINTN                                *SizeOfInfo,
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
);

// 12.9.2.2: EFI_GRAPHICS_OUTPUT_PROTOCOL.SetMode()
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
    EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    uint32_t                     ModeNumber
);

// 12.9.2.3: EFI_GRAPHICS_OUTPUT_PROTOCOL.Blt()
typedef struct {
    uint8_t Blue;
    uint8_t Red;
    uint8_t Green;
    uint8_t Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL      *This,
    IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
    IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,

    IN UINTN SourceX,
    IN UINTN SourceY,
    IN UINTN DestinationX,
    IN UINTN DestinationY,
    IN UINTN Width,
    IN UINTN Height,
    IN UINTN Delta
);

// EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
typedef struct {
    uint32_t MaxMode;
    uint32_t Mode;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    
    UINTN                   SizeOfInfo;
    EFI_PHYSICAL_ADDRESS    FrameBufferBase;
    UINTN                   FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// 12.9.2: EFI_GRAPHICS_OUTPUT_PROTOCOL
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE     QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE       SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT            Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE           *Mode;
};

#endif // GOP_H