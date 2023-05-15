#include "./felica.h"

bool felica_lite_is_issued(FelicaLiteInfo* lite_info);
void felica_describe_system(FelicaSystem* system, FuriString* out);
const char* felica_get_service_type_name(FelicaServiceType type);
const char* felica_get_service_attrib_name(FelicaServiceAttribute attrib, FelicaServiceType type);
void felica_std_describe_node(FelicaNode* node, FuriString* result);
void felica_std_hexdump_blocks(FelicaBlockArray_t blocks, FuriString* out);
void felica_std_cat_service(FelicaService* service, FuriString* out);
void felica_std_describe_node_detailed(FelicaNode* node, FuriString* out);
void felica_print_card_stat(FelicaData* data, FuriString* out);
void felica_print_card_spec(FelicaSpec* spec, FuriString* out);
