// libwolfssl.c
// Defines the entry point for the DLL application

#include <rt.h>

BOOLEAN __stdcall RslMain( RTHANDLE hModule,
                        DWORD  ul_reason_for_call,  
                        LPVOID lpReserved
                       )
{
  switch (ul_reason_for_call) {
    case RSL_PROCESS_ATTACH:
    case RSL_THREAD_ATTACH:
    case RSL_THREAD_DETACH:
    case RSL_PROCESS_DETACH:
    break;
  }

  return TRUE;
}
