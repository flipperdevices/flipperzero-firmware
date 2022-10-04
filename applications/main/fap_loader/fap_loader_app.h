#ifndef FAP_LOADER_APP_H
#define FAP_LPADER_APP_H

typedef struct FapLoader FapLoader;

bool fap_loader_item_callback(string_t path, void* context, uint8_t** icon_ptr, string_t item_name);
FapLoader* fap_loader_alloc_minimal();
void fap_loader_free_minimal(FapLoader* loader);

#endif // FAP_LOADER_APP_H
