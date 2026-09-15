#include <stdint.h>

typedef uint64_t    EFI_STATUS;
typedef void       *EFI_HANDLE;
typedef uint16_t    CHAR16;

typedef struct { 
    uint64_t Signature;
    uint32_t Revision, HeaderSize, CRC32, Reserved;
} EFI_TABLE_HEADER;

struct TextOut;
typedef EFI_STATUS (*EFI_TEXT_STRING)(struct TextOut*, CHAR16*);
typedef EFI_STATUS (*EFI_TEXT_CLEAR)(struct TextOut*);

typedef struct TextOut {
    void *Reset;
    EFI_TEXT_STRING OutputString;
    void *TestString, *QueryMode, *SetMode, *SetAttrib;
    EFI_TEXT_CLEAR ClearScreen;
    void *SetCursorPosition, *EnableCursor, *Mode;
} EFI_TEXT_OUT_PROTOCOL;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16      *FirmwareVendor;
    uint32_t     FirmwareRevision;
    EFI_HANDLE ConsoleInHandle; void *ConIn;
    EFI_HANDLE ConsoleOutHandle; EFI_TEXT_OUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle; void *StdErr;
    void *RuntimeServices;
    void *BootServices;
    uint64_t NumberOfTableEntries;
    void *ConfigurationTable;
} EFI_SYSTEM_TABLE;

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {
    st->ConOut->ClearScreen(st->ConOut);
    st->ConOut->OutputString(st->ConOut, u"Hello, World!\r\n");
    st->ConOut->OutputString(st->ConOut, st->FirmwareVendor);
    for (;;) __asm__ volatile ("hlt");
    return 0;
}