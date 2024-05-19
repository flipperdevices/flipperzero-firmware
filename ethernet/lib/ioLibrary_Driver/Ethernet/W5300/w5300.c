//*****************************************************************************
//
//! \file w5300.h
//! \brief W5300 HAL implement File.
//! \version 1.0.0
//! \date 2015/05/01
//! \par  Revision history
//!       <2015/05/01> 1st Released for integrating with ioLibrary 
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2015, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include <stdint.h>
#include "wizchip_conf.h"

#if _WIZCHIP_ == 5300

   extern uint8_t sock_remained_byte[_WIZCHIP_SOCK_NUM_];
   extern uint8_t sock_pack_info[_WIZCHIP_SOCK_NUM_];


/***********************
 * Basic I/O  Function *
 ***********************/
 
void     WIZCHIP_WRITE(uint32_t AddrSel, uint16_t wb )
{
   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

#if ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_DIR_) )
   #if(_WIZCHIP_IO_BUS_WIDTH_ == 8)
      WIZCHIP.IF.BUS._write_data(AddrSel,  (uint8_t)(wb>>8));
      WIZCHIP.IF.BUS._write_data(WIZCHIP_OFFSET_INC(AddrSel,1),(uint8_t)wb);
   #elif(_WIZCHIP_IO_BUS_WIDTH_ == 16)
      WIZCHIP.IF.BUS._write_data(AddrSel,  wb);   
   #else
      #error "Abnoraml _WIZCHIP_IO_BUS_WIDTH_. Should be 8 or 16"
   #endif
#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_) )
   #if(_WIZCHIP_IO_BUS_WIDTH_ == 8)
      WIZCHIP.IF.BUS._write_data(IDM_AR,                      (uint8_t)(AddrSel >> 8));
      WIZCHIP.IF.BUS._write_data(WIZCHIP_OFFSET_INC(IDM_AR,1),(uint8_t)AddrSel);
      WIZCHIP.IF.BUS._write_data(IDM_DR,(uint8_t)(wb>>8));
      WIZCHIP.IF.BUS._write_data(WIZCHIP_OFFSET_INC(IDM_DR,1),(uint8_t)wb);         
   #elif(_WIZCHIP_IO_BUS_WIDTH_ == 16)
      WIZCHIP.IF.BUS._write_data(IDM_AR, (uint16_t)AddrSel);
      WIZCHIP.IF.BUS._write_data(IDM_DR, wb);
   #else
      #error "Abnoraml _WIZCHIP_IO_BUS_WIDTH_. Should be 8 or 16"
   #endif
#else
   #error "Unknown _WIZCHIP_IO_MODE_ in W5300. !!!"
#endif

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
}

uint16_t WIZCHIP_READ(uint32_t AddrSel)
{
   uint16_t ret;

   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

#if ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_DIR_) )
   #if (_WIZCHIP_IO_BUS_WIDTH_ == 8)
      ret = (((uint16_t)WIZCHIP.IF.BUS._read_data(AddrSel)) << 8) | 
            (((uint16_t)WIZCHIP.IF.BUS._read_data(WIZCHIP_OFFSET_INC(AddrSel,1))) & 0x00FF) ;   
   #elif(_WIZCHIP_IO_BUS_WIDTH_ == 16)
      ret = WIZCHIP.IF.BUS._read_data(AddrSel);
   #else
      #error "Abnoraml _WIZCHIP_IO_BUS_WIDTH_. Should be 8 or 16"
   #endif         
#elif ( (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_) )
   #if(_WIZCHIP_IO_BUS_WIDTH_ == 8)
      WIZCHIP.IF.BUS._write_data(IDM_AR,                      (uint8_t)(AddrSel >> 8));
      WIZCHIP.IF.BUS._write_data(WIZCHIP_OFFSET_INC(IDM_AR,1),(uint8_t)AddrSel);
      ret = (((uint16_t)WIZCHIP.IF.BUS._read_data(IDM_DR)) << 8) | 
            (((uint16_t)WIZCHIP.IF.BUS._read_data(WIZCHIP_OFFSET_INC(IDM_DR,1))) & 0x00FF);
   #elif(_WIZCHIP_IO_BUS_WIDTH_ == 16)
      WIZCHIP.IF.BUS._write_data(IDM_AR, (uint16_t)AddrSel);
      ret = WIZCHIP.IF.BUS._read_data(IDM_DR);
   #else
      #error "Abnoraml _WIZCHIP_IO_BUS_WIDTH_. Should be 8 or 16"
   #endif
#else
   #error "Unknown _WIZCHIP_IO_MODE_ in W5300. !!!"
#endif

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
   return ret;
}


void setTMSR(uint8_t sn,uint8_t tmsr)
{
   uint16_t tmem;
   tmem = WIZCHIP_READ(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE)));
   if(sn & 0x01)  tmem = (tmem & 0xFF00) | (((uint16_t)tmsr ) & 0x00FF) ;
   else tmem =  (tmem & 0x00FF) | (((uint16_t)tmsr) << 8) ;
   WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE)),tmem);
}
   
uint8_t getTMSR(uint8_t sn)
{
   if(sn & 0x01)
      return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE))) & 0x00FF);
   return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(TMS01R, (sn & 0xFE))) >> 8);
}

void setRMSR(uint8_t sn,uint8_t rmsr)
{
   uint16_t rmem;
   rmem = WIZCHIP_READ(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE)));
   if(sn & 0x01)  rmem = (rmem & 0xFF00) | (((uint16_t)rmsr ) & 0x00FF) ;
   else rmem =  (rmem & 0x00FF) | (((uint16_t)rmsr) << 8) ;
   WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE)),rmem);
}
   
uint8_t getRMSR(uint8_t sn)
{
   if(sn & 0x01)
      return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE))) & 0x00FF);
   return (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(RMS01R, (sn & 0xFE))) >> 8);
}

uint32_t getSn_TX_FSR(uint8_t sn)
{
   uint32_t free_tx_size=0;
   uint32_t free_tx_size1=1;
   while(1)
   {
      free_tx_size = (((uint32_t)WIZCHIP_READ(Sn_TX_FSR(sn))) << 16) | 
                     (((uint32_t)WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),2))) & 0x0000FFFF);                           // read
      if(free_tx_size == free_tx_size1) break;  // if first == sencond, Sn_TX_FSR value is valid.                                                          
      free_tx_size1 = free_tx_size;             // save second value into first                                                   
   }                                                                       
   return free_tx_size;                                                    
}                                                                          

uint32_t getSn_RX_RSR(uint8_t sn)
{
   uint32_t received_rx_size=0;
   uint32_t received_rx_size1=1;
   while(1)
   {
      received_rx_size = (((uint32_t)WIZCHIP_READ(Sn_RX_RSR(sn))) << 16) | 
                         (((uint32_t)WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),2))) & 0x0000FFFF);
      if(received_rx_size == received_rx_size1) break;                                                                         
      received_rx_size1 = received_rx_size;                                      // if first == sencond, Sn_RX_RSR value is valid.
   }                                                                             // save second value into first                
   return received_rx_size + (uint32_t)((sock_pack_info[sn] & 0x02) ? 1 : 0);   
}


void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint32_t len)
{
   uint32_t i = 0;
   if(len == 0)  return;
   
   for(i = 0; i < len ; i += 2)
      setSn_TX_FIFOR(sn, (((uint16_t)wizdata[i]) << 8) | (((uint16_t)wizdata[i+1]) & 0x00FF))
}

void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint32_t len)
{
   uint16_t rd = 0;
   uint32_t i = 0;
   
   if(len == 0) return;
      
   for(i = 0; i < len; i++)
   {
      if((i & 0x01)==0)
      {
         rd = getSn_RX_FIFOR(sn);
         wizdata[i]   = (uint8_t)(rd >> 8);
      }
      else  wizdata[i] = (uint8_t)rd;  // For checking the memory access violation
   }
   sock_remained_byte[sn] = (uint8_t)rd; // back up the remaind fifo byte.
}

void wiz_recv_ignore(uint8_t sn, uint32_t len)
{
   uint32_t i = 0;
   for(i = 0; i < len ; i += 2) getSn_RX_FIFOR(sn);
}


#endif
