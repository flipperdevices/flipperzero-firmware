#include "app_methods.h"

static constexpr auto module_demo_api_table = sort(create_array_t<sym_entry>(
    API_METHOD(app_api_accumulator_set, void, (uint32_t)),
    API_METHOD(app_api_accumulator_get, uint32_t, ()),
    API_METHOD(app_api_accumulator_add, void, (uint32_t)),
    API_METHOD(app_api_accumulator_sub, void, (uint32_t)),
    API_METHOD(app_api_accumulator_mul, void, (uint32_t))));