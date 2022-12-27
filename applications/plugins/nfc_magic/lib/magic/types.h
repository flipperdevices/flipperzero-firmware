#pragma once

typedef enum {
    MagicTypeClassicGen1A,
    MagicTypeClassicGen1B,
    MagicTypeClassicDirectWrite,
    MagicTypeClassicAPDU,
    MagicTypeUltralightGen1,
    MagicTypeUltralightDirectWrite,
    MagicTypeUltralightC_Gen1,
    MagicTypeUltralightC_DirectWrite,
    MagicTypeGen4,
} MagicType;

const char* nfc_magic_type(MagicType type);