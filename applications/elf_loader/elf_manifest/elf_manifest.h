#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ELF_MANIFEST_MAGIC 0x52474448

#pragma pack(push, 1)

typedef struct {
    uint32_t manifest_magic;
    uint32_t manifest_version;
    union {
        struct {
            uint16_t major;
            uint16_t minor;
        };
        uint32_t version;
    } api_version;
} ElfManifestBase;

typedef struct {
    uint16_t stack_size;
    uint32_t app_version;
    char name[32];
} ElfManifestV1;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif