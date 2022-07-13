#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ELF_MANIFEST_MAGIC 0x52474448
#define ELF_MANIFEST_MAX_SUPPORTED_VERSION 1

#pragma pack(push, 1)

typedef struct {
    uint32_t manifest_magic;
    uint32_t manifest_version;
    union {
        struct {
            uint16_t minor;
            uint16_t major;
        };
        uint32_t version;
    } api_version;
} ElfManifestBase;

typedef struct {
    ElfManifestBase base;

    uint16_t stack_size;
    uint32_t app_version;
    char name[32];
} ElfManifestV1;

typedef ElfManifestV1 ElfManifest;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
