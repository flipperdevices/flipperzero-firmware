/* @page License
 *
 * Copyright (c) 2020 Fabien MAILLY
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
#ifndef CONF_MCP251XFD_H
#define CONF_MCP251XFD_H
//=============================================================================

//-----------------------------------------------------------------------------





// If in debug mode, check NULL parameters that are mandatory in each functions of the driver
#ifdef DEBUG
#  define CHECK_NULL_PARAM
#endif

// This define the max size for the transfer buffer. Adjust for 1 max full frame that is needed of all controllers in use otherwise, the message will be cut into parts that slow the transfer
// In case of use write safe by all controllers set 9, which is the minimum allowed, because all data will be send by 4 bytes max
#define MCP251XFD_TRANS_BUF_SIZE  ( 2+1+8+64+4+2 ) // here it is set to 1 full CAN-FD frame with CRC (2 for Cmd + 1 for length + 8 for CAN frame + 64 for CAN-FD max payload + 4 for time stamp + 2 for CRC)





//-----------------------------------------------------------------------------
#endif /* CONF_MCP251XFD_H */
