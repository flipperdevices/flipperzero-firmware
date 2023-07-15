#include "./felica.h"

#ifdef __cplusplus
extern "C" {
#endif

uint_least32_t felica_estimate_timing_us(uint_least8_t timing, uint_least8_t units);
bool felica_lite_is_issued(FelicaLiteInfo* lite_info);
FuriString* felica_get_system_name(FelicaSystem* system);
FuriString* felica_get_service_name(FelicaService* service);

#ifdef __cplusplus
}
#endif