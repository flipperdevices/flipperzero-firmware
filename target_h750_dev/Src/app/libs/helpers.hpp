#pragma once

template< typename T_from,  typename T_to>
T_to* remove_const_cast( const T_from* ptr )
{
    return reinterpret_cast<T_to*>(const_cast<T_from*>(ptr));
}
