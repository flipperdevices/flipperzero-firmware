/*******************************************************************************
  File name:    ShowDemoData.h
  Author:       FMA
  Version:      1.0
  Date (d/m/y): 29/04/2020
  Description:  Some functions to show on console for the DEMO

  History :
*******************************************************************************/
#ifndef SHOWDEMODATA_H_
#define SHOWDEMODATA_H_
//=============================================================================

//-----------------------------------------------------------------------------
/// @cond 0
/**INDENT-OFF**/
#include "stdio.h"
#include <stdarg.h>
#include "../mcp251xfd/MCP251XFD.h"
#include "../mcp251xfd/ErrorsDef.h"
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond
//-----------------------------------------------------------------------------

/*! @brief Show which device is detected on the console
 *
 * @param[in] *pComp Is the pointed structure of the device connected
 * @param[in] sysclk Is the system clock of the component (Usually MCP251XFD_Config.SYSCLK_Result)
 * @return Returns an #eERRORRESULT value enum
 */
void show_device_detected(MCP251XFD* pComp, uint32_t sysclk);

/*! @brief Show device configuration on the console
 *
 * It will show bitrates and other informations
 * @param[in] *btStats Is the device bit time configuration of the component (Usually MCP251XFD_Config.BitTimeStats)
 * @return Returns an #eERRORRESULT value enum
 */
void show_device_configuration(MCP251XFD_BitTimeStats* btStats);

/*! @brief Show device FIFO configuration
 *
 * It will show the device FIFO configuration in an ASCII formatted table
 * @param[in] *listFIFO Is the FIFO list of the device to show
 * @param[in] count Is the count of elements in the FIFO list
 * @return Returns an #eERRORRESULT value enum
 */
void show_device_fifo_configuration(MCP251XFD_FIFO* listFIFO, size_t count);

/*! @brief Show device Filter configuration
 *
 * It will show the device Filter configuration in an ASCII formatted table
 * @param[in] *listFilter Is the filter list of the device to show
 * @param[in] count Is the count of elements in the filter list
 * @param[in] UseSID11 Indicate if the device use the SID11 bit in FDmode
 * @return Returns an #eERRORRESULT value enum
 */
void show_device_filter_configuration(MCP251XFD_Filter* listFilter, size_t count, bool UseSID11);

//-----------------------------------------------------------------------------
/*! @brief Show the current error of a device
 *
 * @param[in] *pComp Is the pointed structure of the device in error
 * @param[in] error Is the error to show on console
 * @return Returns an #eERRORRESULT value enum
 */
void show_device_error(MCP251XFD* pComp, eERRORRESULT error);

//-----------------------------------------------------------------------------
void show_device_show_mcp251xfd_sfr_reg(MCP251XFD* pComp);

void show_device_show_mcp251xfd_can_sfr_reg(MCP251XFD* pComp);

void show_device_show_mcp251xfd_fifo_reg(MCP251XFD* pComp);

void show_device_show_mcp251xfd_filter_reg(MCP251XFD* pComp);

//-----------------------------------------------------------------------------
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
//-----------------------------------------------------------------------------
#endif /* SHOWDEMODATA_H_ */