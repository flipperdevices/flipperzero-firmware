#include "./felica.h"

uint_least32_t felica_estimate_timing_us(uint_least8_t timing, uint_least8_t units);
FuriString* felica_get_system_name(FelicaSystem* system);
FuriString* felica_get_service_name(FelicaService* service);
const char* felica_get_service_type_name(FelicaServiceType type);
const char* felica_get_service_attrib_name(FelicaServiceAttribute attrib, FelicaServiceType type);
void felica_std_describe_node(FelicaNode* node, FuriString* result);
void felica_std_cat_service(FelicaService* service, FuriString* out);
void felica_std_describe_node_detailed(FelicaNode* node, FuriString* out);