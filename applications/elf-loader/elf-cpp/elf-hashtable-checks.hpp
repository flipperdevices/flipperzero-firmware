#pragma once

#include <array>

template <std::size_t N>
constexpr bool has_hash_collisions(const std::array<sym_entry, N> api_methods) {
    for(std::size_t i = 0; i < (N - 1); ++i) {
        if(api_methods[i].hash == api_methods[i + 1].hash) {
            return true;
        }
    }

    return false;
}
