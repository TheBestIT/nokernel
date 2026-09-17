#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <stddef.h>

typedef uint64_t    UINTN;
typedef int64_t     INTN;
typedef UINTN       EFI_STATUS;
typedef void       *EFI_HANDLE;
typedef uint16_t    CHAR16;

#define NULL            ((void *) 0)
#define EFI_ERROR(a)    (((INTN) a) < 0)
#define EFIERR(a)       (0x8000000000000000 | a)

typedef struct {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} EFI_GUID;

typedef struct { 
    UINTN Signature;
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
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;


#endif // BASE_H