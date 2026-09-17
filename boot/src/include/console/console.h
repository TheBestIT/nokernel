#ifndef CONSOLE_H
#define CONSOLE_H

#include "efiapi/Protocols.h"

void ClearScreen(EFI_SYSTEM_TABLE *st);
void Print(EFI_SYSTEM_TABLE *st, CHAR16 *string);

void GuidToString(CHAR16 out[37], const EFI_GUID *guid);

#endif // CONSOLE_H