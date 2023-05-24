#include "./felica.h"

uint_least32_t felica_estimate_timing_us(uint_least8_t timing, uint_least8_t units);
FuriString* felica_get_system_name(FelicaSystem* system);
FuriString* felica_get_service_name(FelicaService* service);