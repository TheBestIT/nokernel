#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Base.h"

#define EFI_FILE_MODE_READ  0x0000000000000001ULL

// Forward declarations. The protocol structs and their function pointers
// refer to each other. Declare the struct tags first to break the cycle.
typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// 13.5.2: EFI_FILE_PROTOCOL.Open()
typedef EFI_STATUS (*EFI_FILE_OPEN) (
    EFI_FILE_PROTOCOL*,     // This
    EFI_FILE_PROTOCOL**,    // NewHandle
    CHAR16*,                // Filename
    UINTN,                  // OpenMode
    UINTN                   // Attributes 
);

// 13.5.3: EFI_FILE_PROTOCOL.Close()
typedef EFI_STATUS (*EFI_FILE_CLOSE) (
    EFI_FILE_PROTOCOL* // This
);

// 13.5.4 EFI_FILE_PROTOCOL.Delete()
typedef EFI_STATUS (*EFI_FILE_DELETE) (
    EFI_FILE_PROTOCOL* // This
);

// 13.5.5: EFI_FILE_PROTOCOL.Read()
typedef EFI_STATUS (*EFI_FILE_READ) (
    EFI_FILE_PROTOCOL*, // This
    UINTN*,             // BufferSize
    void*               // Buffer
);

// 13.5.6: EFI_FILE_PROTOCOL.Write()
typedef EFI_STATUS (*EFI_FILE_WRITE) (
    EFI_FILE_PROTOCOL*, // This
    UINTN*,             // BufferSize
    void*               // Buffer
);

// 13.5.7: EFI_FILE_PROTOCOL.OpenEx()
typedef void *EFI_EVENT;

typedef struct {
    EFI_EVENT       Event;
    EFI_STATUS      Status;
    UINTN           BufferSize;
    void            *Buffer;
} EFI_FILE_IO_TOKEN;

typedef EFI_STATUS (*EFI_FILE_OPEN_EX) (
    EFI_FILE_PROTOCOL*,  // This
    EFI_FILE_PROTOCOL**, // NewHandle
    CHAR16*,             // Filename
    UINTN,               // OpenMode
    UINTN                // Attributes
);

// 13.5.8: EFI_FILE_PROTOCOL.ReadEx()
typedef EFI_STATUS (*EFI_FILE_READ_EX) (
    EFI_FILE_PROTOCOL*, // This
    EFI_FILE_IO_TOKEN* // Token
);

// 13.5.9: EFI_FILE_PROTOCOL.WriteEx()
typedef EFI_STATUS (*EFI_FILE_WRITE_EX) (
    EFI_FILE_PROTOCOL*, // This
    EFI_FILE_IO_TOKEN*  // Token
);

// 13.5.10: EFI_FILE_PROTOCOL.FlushEx()
typedef EFI_STATUS (*EFI_FILE_FLUSH_EX) (
    EFI_FILE_PROTOCOL*, // This
    EFI_FILE_IO_TOKEN*  // Token
);

// 13.5.11: EFI_FILE_PROTOCOL.SetPosition()
typedef EFI_STATUS (*EFI_FILE_SET_POSITION) (
    EFI_FILE_PROTOCOL*, // This
    UINTN               // Position
);

// 13.5.12: EFI_FILE_PROTOCOL.GetPosition()
typedef EFI_STATUS (*EFI_FILE_GET_POSITION) (
    EFI_FILE_PROTOCOL*, // This
    UINTN*              // Position
);

// 13.5.13: EFI_FILE_PROTOCOL.GetInfo()
typedef EFI_STATUS (*EFI_FILE_GET_INFO) (
    EFI_FILE_PROTOCOL*, // This
    EFI_GUID*,          // InformationType
    UINTN*,             // BufferSize
    void*               // Buffer
);

// 13.5.14: EFI_FILE_PROTOCOL.SetInfo()
typedef EFI_STATUS (*EFI_FILE_SET_INFO) (
    EFI_FILE_PROTOCOL*, // This
    EFI_GUID*,          // InformationType
    UINTN,              // BufferSize
    void*               // Buffer
);

// 13.5.15: EFI_FILE_PROTOCOL.Flush()
typedef EFI_STATUS (*EFI_FILE_FLUSH) (
    EFI_FILE_PROTOCOL* // This
);

// 13.5.16: EFI_FILE_INFO
typedef struct {
    UINTN       Size;
    UINTN       FileSize;
    UINTN       PhysicalSize;
    EFI_TIME    CreateTime;
    EFI_TIME    LastAccessTime;
    EFI_TIME    ModificationTime;
    UINTN       Attribute;
    CHAR16      FileName[];
} EFI_FILE_INFO;

// From UEFI Spec 13.5.1: EFI_FILE_PROTOCOL
struct _EFI_FILE_PROTOCOL {
    UINTN                   Revision;
    EFI_FILE_OPEN           Open;
    EFI_FILE_CLOSE          Close;
    EFI_FILE_DELETE         Delete;
    EFI_FILE_READ           Read;
    EFI_FILE_WRITE          Write;
    EFI_FILE_GET_POSITION   GetPosition;
    EFI_FILE_SET_POSITION   SetPosition;
    EFI_FILE_GET_INFO       GetInfo;
    EFI_FILE_SET_INFO       SetInfo;
    EFI_FILE_FLUSH          Flush;
    EFI_FILE_OPEN_EX        OpenEx;
    EFI_FILE_READ_EX        ReadEx;
    EFI_FILE_WRITE_EX       WriteEx;
    EFI_FILE_FLUSH_EX       FlushEx;
};

// From UEFI Spec 13.4.2: EFI_SIMPLE_FILE SYSTEM_PROTOCOL.OpenVolume()
typedef EFI_STATUS (*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME) (
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*, // This
    EFI_FILE_PROTOCOL**               // Out -> Root
);

// From UEFI Spec 13.4.1: EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINTN Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
};

#endif // FILESYSTEM_H