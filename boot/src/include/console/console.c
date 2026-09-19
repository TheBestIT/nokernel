#include "console.h"

void ClearScreen(EFI_SYSTEM_TABLE *st) {
    st->ConOut->ClearScreen(st->ConOut);
}

void Print(EFI_SYSTEM_TABLE *st, CHAR16 *string) {
    st->ConOut->OutputString(st->ConOut, string);
}

void PrintHex(EFI_SYSTEM_TABLE *st, uint64_t v) {
    static const CHAR16 hex[] = u"0123456789ABCDEF";
    CHAR16 buf[19];

    buf[0] = u'0';
    buf[1] = u'x';
    for (int i = 0; i < 16; i++)
        buf[2 + i] = hex[(v >> (60 - i * 4)) & 0xf];
    buf[18] = 0;

    Print(st, buf);
}

void GuidToString(CHAR16 out[37], const EFI_GUID *guid) {
    static const CHAR16 hex[] = u"0123456789ABCDEF";
    UINTN i = 0;
    #define NIB(v, s) (out[i++] = hex[((v) >> (s)) & 0xf])

    for (int s = 28; s >= 0; s -= 4) NIB(guid->Data1, s);
    out[i++] = u'-';
    for (int s = 12; s >= 0; s -= 4) NIB(guid->Data2, s);
    out[i++] = u'-';
    for (int s = 12; s >= 0; s -= 4) NIB(guid->Data3, s);
    out[i++] = u'-';
    for (int b = 0; b < 2; b++) { NIB(guid->Data4[b], 4); NIB(guid->Data4[b], 0); }
    out[i++] = u'-';
    for (int b = 2; b < 8; b++) { NIB(guid->Data4[b], 4); NIB(guid->Data4[b], 0); }
    out[i] = 0;
    #undef NIB
}