#ifndef MEDIA_H
#define MEDIA_H

#include "Base.h"
#include "Protocols.h"

#define MEDIA_DEVICE_PATH      0x04
#define MEDIA_HARDDRIVE_DP     0x01
#define SIGNATURE_TYPE_MBR     0x01
#define SIGNATURE_TYPE_GUID    0x02

// From UEFI Spec 10.3.6: Hard Drive Media Device Path
typedef struct {
    EFI_DEVICE_PATH_PROTOCOL Header;
    uint32_t PartitionNumber;
    uint64_t PartitionStart;
    uint64_t PartitionSize;
    uint8_t  Signature[16];
    uint8_t  MBRType;
    uint8_t  SignatureType;
} __attribute__((packed)) HARDDRIVE_DEVICE_PATH;

#endif // MEDIA_H