#pragma once

template< typename T_from,  typename T_to>
T_to* remove_const_cast( const T_from* ptr )
{
    return reinterpret_cast<T_to*>(const_cast<T_from*>(ptr));
}

// FNV-1a hash, 32-bit 
inline constexpr std::uint32_t strhash(const char* str, std::uint32_t hash = 2166136261UL) {
    return *str ? strhash(str + 1, (hash ^ *str) * 16777619ULL) : hash;
}