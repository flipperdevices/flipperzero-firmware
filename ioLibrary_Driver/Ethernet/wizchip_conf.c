//****************************************************************************/ 
//!
//! \file wizchip_conf.c
//! \brief WIZCHIP Config Header File.
//! \version 1.0.1
//! \date 2013/10/21
//! \par  Revision history
//!       <2015/02/05> Notice
//!        The version history is not updated after this point.
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//!       <2014/05/01> V1.0.1  Refer to M20140501
//!        1. Explicit type casting in wizchip_bus_readdata() & wizchip_bus_writedata()
//            Issued by Mathias ClauBen.
//!           uint32_t type converts into ptrdiff_t first. And then recoverting it into uint8_t*
//!           For remove the warning when pointer type size is not 32bit.
//!           If ptrdiff_t doesn't support in your complier, You should must replace ptrdiff_t into your suitable pointer type.
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
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
//*****************************************************************************/
//A20140501 : for use the type - ptrdiff_t
#include <stddef.h>
//

#include "wizchip_conf.h"

/////////////
//M20150401 : Remove ; in the default callback function such as wizchip_cris_enter(), wizchip_cs_select() and etc.
/////////////

/**
 * @brief Default function to enable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	  wizchip_cris_enter(void)           {};
void 	  wizchip_cris_enter(void)           {}

/**
 * @brief Default function to disable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	  wizchip_cris_exit(void)          {};
void 	  wizchip_cris_exit(void)          {}

/**
 * @brief Default function to select chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_cs_select(void)            {};
void 	wizchip_cs_select(void)            {}

/**
 * @brief Default function to deselect chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_cs_deselect(void)          {};
void 	wizchip_cs_deselect(void)          {}

/**
 * @brief Default function to read in direct or indirect interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
 //M20150601 : Rename the function for integrating with W5300
//uint8_t wizchip_bus_readbyte(uint32_t AddrSel) { return * ((volatile uint8_t *)((ptrdiff_t) AddrSel)); }
iodata_t wizchip_bus_readdata(uint32_t AddrSel) { return * ((volatile iodata_t *)((ptrdiff_t) AddrSel)); }

/**
 * @brief Default function to write in direct or indirect interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//M20150601 : Rename the function for integrating with W5300
//void 	wizchip_bus_writebyte(uint32_t AddrSel, uint8_t wb)  { *((volatile uint8_t*)((ptrdiff_t)AddrSel)) = wb; }
void 	wizchip_bus_writedata(uint32_t AddrSel, iodata_t wb)  { *((volatile iodata_t*)((ptrdiff_t)AddrSel)) = wb; }

/**
 * @brief Default function to read in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//uint8_t wizchip_spi_readbyte(void)        {return 0;};
uint8_t wizchip_spi_readbyte(void)        {return 0;}

/**
 * @brief Default function to write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_spi_writebyte(uint8_t wb) {};
void 	wizchip_spi_writebyte(uint8_t wb) {}

/**
 * @brief Default function to burst read in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_spi_readburst(uint8_t* pBuf, uint16_t len) 	{}; 
void 	wizchip_spi_readburst(uint8_t* pBuf, uint16_t len) 	{}

/**
 * @brief Default function to burst write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {};
void 	wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {}

/**
 * @\ref _WIZCHIP instance
 */
//
//M20150401 : For a compiler didnot support a member of structure
//            Replace the assignment of struct members with the assingment of array
//
/*
_WIZCHIP  WIZCHIP =
      {
      .id                  = _WIZCHIP_ID_,
      .if_mode             = _WIZCHIP_IO_MODE_,
      .CRIS._enter         = wizchip_cris_enter,
      .CRIS._exit          = wizchip_cris_exit,
      .CS._select          = wizchip_cs_select,
      .CS._deselect        = wizchip_cs_deselect,
      .IF.BUS._read_byte   = wizchip_bus_readbyte,
      .IF.BUS._write_byte  = wizchip_bus_writebyte
//    .IF.SPI._read_byte   = wizchip_spi_readbyte,
//    .IF.SPI._write_byte  = wizchip_spi_writebyte
      };
*/      
_WIZCHIP  WIZCHIP =
{
    _WIZCHIP_IO_MODE_,
    _WIZCHIP_ID_ ,
    {
        wizchip_cris_enter,
        wizchip_cris_exit
    },
    {
        wizchip_cs_select,
        wizchip_cs_deselect
    },
    {
        {
            //M20150601 : Rename the function 
            //wizchip_bus_readbyte,
            //wizchip_bus_writebyte
            wizchip_bus_readdata,
            wizchip_bus_writedata
        },

    }
};


static uint8_t    _DNS_[4];      // DNS server ip address
static dhcp_mode  _DHCP_;        // DHCP mode

void reg_wizchip_cris_cbfunc(void(*cris_en)(void), void(*cris_ex)(void))
{
   if(!cris_en || !cris_ex)
   {
      WIZCHIP.CRIS._enter = wizchip_cris_enter;
      WIZCHIP.CRIS._exit  = wizchip_cris_exit;
   }
   else
   {
      WIZCHIP.CRIS._enter = cris_en;
      WIZCHIP.CRIS._exit  = cris_ex;
   }
}

void reg_wizchip_cs_cbfunc(void(*cs_sel)(void), void(*cs_desel)(void))
{
   if(!cs_sel || !cs_desel)
   {
      WIZCHIP.CS._select   = wizchip_cs_select;
      WIZCHIP.CS._deselect = wizchip_cs_deselect;
   }
   else
   {
      WIZCHIP.CS._select   = cs_sel;
      WIZCHIP.CS._deselect = cs_desel;
   }
}

//M20150515 : For integrating with W5300
//void reg_wizchip_bus_cbfunc(uint8_t(*bus_rb)(uint32_t addr), void (*bus_wb)(uint32_t addr, uint8_t wb))
void reg_wizchip_bus_cbfunc(iodata_t(*bus_rb)(uint32_t addr), void (*bus_wb)(uint32_t addr, iodata_t wb))
{
   while(!(WIZCHIP.if_mode & _WIZCHIP_IO_MODE_BUS_));
   //M20150601 : Rename call back function for integrating with W5300
   /*
   if(!bus_rb || !bus_wb)
   {
      WIZCHIP.IF.BUS._read_byte   = wizchip_bus_readbyte;
      WIZCHIP.IF.BUS._write_byte  = wizchip_bus_writebyte;
   }
   else
   {
      WIZCHIP.IF.BUS._read_byte   = bus_rb;
      WIZCHIP.IF.BUS._write_byte  = bus_wb;
   }
   */
   if(!bus_rb || !bus_wb)
   {
      WIZCHIP.IF.BUS._read_data   = wizchip_bus_readdata;
      WIZCHIP.IF.BUS._write_data  = wizchip_bus_writedata;
   }
   else
   {
      WIZCHIP.IF.BUS._read_data   = bus_rb;
      WIZCHIP.IF.BUS._write_data  = bus_wb;
   }
}

void reg_wizchip_spi_cbfunc(uint8_t (*spi_rb)(void), void (*spi_wb)(uint8_t wb))
{
   while(!(WIZCHIP.if_mode & _WIZCHIP_IO_MODE_SPI_));
   
   if(!spi_rb || !spi_wb)
   {
      WIZCHIP.IF.SPI._read_byte   = wizchip_spi_readbyte;
      WIZCHIP.IF.SPI._write_byte  = wizchip_spi_writebyte;
   }
   else
   {
      WIZCHIP.IF.SPI._read_byte   = spi_rb;
      WIZCHIP.IF.SPI._write_byte  = spi_wb;
   }
}

// 20140626 Eric Added for SPI burst operations
void reg_wizchip_spiburst_cbfunc(void (*spi_rb)(uint8_t* pBuf, uint16_t len), void (*spi_wb)(uint8_t* pBuf, uint16_t len))
{
   while(!(WIZCHIP.if_mode & _WIZCHIP_IO_MODE_SPI_));

   if(!spi_rb || !spi_wb)
   {
      WIZCHIP.IF.SPI._read_burst   = wizchip_spi_readburst;
      WIZCHIP.IF.SPI._write_burst  = wizchip_spi_writeburst;
   }
   else
   {
      WIZCHIP.IF.SPI._read_burst   = spi_rb;
      WIZCHIP.IF.SPI._write_burst  = spi_wb;
   }
}

int8_t ctlwizchip(ctlwizchip_type cwtype, void* arg)
{
#if	_WIZCHIP_ == W5100S || _WIZCHIP_ == W5200 || _WIZCHIP_ == W5500
   uint8_t tmp = 0;
#endif
   uint8_t* ptmp[2] = {0,0};
   switch(cwtype)
   {
      case CW_RESET_WIZCHIP:
         wizchip_sw_reset();
         break;
      case CW_INIT_WIZCHIP:
         if(arg != 0) 
         {
            ptmp[0] = (uint8_t*)arg;
            ptmp[1] = ptmp[0] + _WIZCHIP_SOCK_NUM_;
         }
         return wizchip_init(ptmp[0], ptmp[1]);
      case CW_CLR_INTERRUPT:
         wizchip_clrinterrupt(*((intr_kind*)arg));
         break;
      case CW_GET_INTERRUPT:
        *((intr_kind*)arg) = wizchip_getinterrupt();
         break;
      case CW_SET_INTRMASK:
         wizchip_setinterruptmask(*((intr_kind*)arg));
         break;         
      case CW_GET_INTRMASK:
         *((intr_kind*)arg) = wizchip_getinterruptmask();
         break;
   //M20150601 : This can be supported by W5200, W5500
   //#if _WIZCHIP_ > W5100
   #if (_WIZCHIP_ == W5200 || _WIZCHIP_ == W5500)
      case CW_SET_INTRTIME:
         setINTLEVEL(*(uint16_t*)arg);
         break;
      case CW_GET_INTRTIME:
         *(uint16_t*)arg = getINTLEVEL();
         break;
   #endif
      case CW_GET_ID:
         ((uint8_t*)arg)[0] = WIZCHIP.id[0];
         ((uint8_t*)arg)[1] = WIZCHIP.id[1];
         ((uint8_t*)arg)[2] = WIZCHIP.id[2];
         ((uint8_t*)arg)[3] = WIZCHIP.id[3];
         ((uint8_t*)arg)[4] = WIZCHIP.id[4];
         ((uint8_t*)arg)[5] = WIZCHIP.id[5];
         ((uint8_t*)arg)[6] = 0;
         break;
   #if _WIZCHIP_ == W5100S || _WIZCHIP_ == W5500
      case CW_RESET_PHY:
         wizphy_reset();
         break;
      case CW_SET_PHYCONF:
         wizphy_setphyconf((wiz_PhyConf*)arg);
         break;
      case CW_GET_PHYCONF:
         wizphy_getphyconf((wiz_PhyConf*)arg);
         break;
      case CW_GET_PHYSTATUS:
         break;
      case CW_SET_PHYPOWMODE:
         return wizphy_setphypmode(*(uint8_t*)arg);
   #endif
   #if _WIZCHIP_ == W5100S || _WIZCHIP_ == W5200 || _WIZCHIP_ == W5500
      case CW_GET_PHYPOWMODE:
         tmp = wizphy_getphypmode();
         if((int8_t)tmp == -1) return -1;
         *(uint8_t*)arg = tmp;
         break;
      case CW_GET_PHYLINK:
         tmp = wizphy_getphylink();
         if((int8_t)tmp == -1) return -1;
         *(uint8_t*)arg = tmp;
         break;
   #endif      
      default:
         return -1;
   }
   return 0;
}


int8_t ctlnetwork(ctlnetwork_type cntype, void* arg)
{
   
   switch(cntype)
   {
      case CN_SET_NETINFO:
         wizchip_setnetinfo((wiz_NetInfo*)arg);
         break;
      case CN_GET_NETINFO:
         wizchip_getnetinfo((wiz_NetInfo*)arg);
         break;
      case CN_SET_NETMODE:
         return wizchip_setnetmode(*(netmode_type*)arg);
      case CN_GET_NETMODE:
         *(netmode_type*)arg = wizchip_getnetmode();
         break;
      case CN_SET_TIMEOUT:
         wizchip_settimeout((wiz_NetTimeout*)arg);
         break;
      case CN_GET_TIMEOUT:
         wizchip_gettimeout((wiz_NetTimeout*)arg);
         break;
      default:
         return -1;
   }
   return 0;
}

void wizchip_sw_reset(void)
{
   uint8_t gw[4], sn[4], sip[4];
   uint8_t mac[6];
//A20150601
#if _WIZCHIP_IO_MODE_  == _WIZCHIP_IO_MODE_BUS_INDIR_
   uint16_t mr = (uint16_t)getMR();
   setMR(mr | MR_IND);
#endif
//
   getSHAR(mac);
   getGAR(gw);  getSUBR(sn);  getSIPR(sip);
   setMR(MR_RST);
   getMR(); // for delay
//A2015051 : For indirect bus mode 
#if _WIZCHIP_IO_MODE_  == _WIZCHIP_IO_MODE_BUS_INDIR_
   setMR(mr | MR_IND);
#endif
//
   setSHAR(mac);
   setGAR(gw);
   setSUBR(sn);
   setSIPR(sip);
}

int8_t wizchip_init(uint8_t* txsize, uint8_t* rxsize)
{
   int8_t i;
#if _WIZCHIP_ < W5200
   int8_t j;
#endif
   int8_t tmp = 0;
   wizchip_sw_reset();
   if(txsize)
   {
      tmp = 0;
   //M20150601 : For integrating with W5300
   #if _WIZCHIP_ == W5300
		for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
		{
			if(txsize[i] >= 64) return -1;   //No use 64KB even if W5300 support max 64KB memory allocation
			tmp += txsize[i];
			if(tmp > 128) return -1;
		}
		if(tmp % 8) return -1;
   #else
		for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
		{
			tmp += txsize[i];

		#if _WIZCHIP_ < W5200	//2016.10.28 peter add condition for w5100 and w5100s
			if(tmp > 8) return -1;
		#else
			if(tmp > 16) return -1;
		#endif
		}
		for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
		{
		#if _WIZCHIP_ < W5200	//2016.10.28 peter add condition for w5100
			j = 0;
			while((txsize[i] >> j != 1)&&(txsize[i] !=0)){j++;}
			setSn_TXBUF_SIZE(i, j);
		#else
			setSn_TXBUF_SIZE(i, txsize[i]);
		#endif
		}

	#endif
   }

   if(rxsize)
   {
      tmp = 0;
   #if _WIZCHIP_ == W5300
      for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
		{
			if(rxsize[i] >= 64) return -1;   //No use 64KB even if W5300 support max 64KB memory allocation
			tmp += rxsize[i];
			if(tmp > 128) return -1;
		}
		if(tmp % 8) return -1;
   #else
		for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
		{
			tmp += rxsize[i];
		#if _WIZCHIP_ < W5200	//2016.10.28 peter add condition for w5100 and w5100s
			if(tmp > 8) return -1;
		#else
			if(tmp > 16) return -1;
		#endif
		}

		for(i = 0 ; i < _WIZCHIP_SOCK_NUM_; i++)
		{
		#if _WIZCHIP_ < W5200	// add condition for w5100
			j = 0;
			while((rxsize[i] >> j != 1)&&(txsize[i] !=0)){j++;}
			setSn_RXBUF_SIZE(i, j);
		#else
			setSn_RXBUF_SIZE(i, rxsize[i]);
		#endif
		}
	#endif
   }
   return 0;
}

void wizchip_clrinterrupt(intr_kind intr)
{
   uint8_t ir  = (uint8_t)intr;
   uint8_t sir = (uint8_t)((uint16_t)intr >> 8);
#if _WIZCHIP_ < W5500
   ir |= (1<<4); // IK_WOL
#endif
#if _WIZCHIP_ == W5200
   ir |= (1 << 6);
#endif
   
#if _WIZCHIP_ < W5200
   sir &= 0x0F;
#endif

#if _WIZCHIP_ <= W5100S
   ir |= sir;
   setIR(ir);
//A20150601 : For integrating with W5300
#elif _WIZCHIP_ == W5300
   setIR( ((((uint16_t)ir) << 8) | (((uint16_t)sir) & 0x00FF)) );
#else
   setIR(ir);
//M20200227 : For clear
   //setSIR(sir);
   for(ir=0; ir<8; ir++){
       if(sir & (0x01 <<ir) ) setSn_IR(ir, 0xff);
   }

#endif   
}

intr_kind wizchip_getinterrupt(void)
{
   uint8_t ir  = 0;
   uint8_t sir = 0;
   uint16_t ret = 0;
#if _WIZCHIP_ <= W5100S
   ir = getIR();
   sir = ir & 0x0F;
//A20150601 : For integrating with W5300
#elif _WIZCHIP_  == W5300
   ret = getIR();
   ir = (uint8_t)(ret >> 8);
   sir = (uint8_t)ret;
#else
   ir  = getIR();
   sir = getSIR();
#endif         

//M20150601 : For Integrating with W5300
//#if _WIZCHIP_ < W5500
#if _WIZCHIP_ < W5200
   ir &= ~(1<<4); // IK_WOL
#endif
#if _WIZCHIP_ == W5200
   ir &= ~(1 << 6);
#endif
  ret = sir;
  ret = (ret << 8) + ir;
  return (intr_kind)ret;
}

void wizchip_setinterruptmask(intr_kind intr)
{
   uint8_t imr  = (uint8_t)intr;
   uint8_t simr = (uint8_t)((uint16_t)intr >> 8);
#if _WIZCHIP_ < W5500
   imr &= ~(1<<4); // IK_WOL
#endif
#if _WIZCHIP_ == W5200
   imr &= ~(1 << 6);
#endif
   
#if _WIZCHIP_ < W5200
   simr &= 0x0F;
   imr |= simr;
   setIMR(imr);
//A20150601 : For integrating with W5300
#elif _WIZCHIP_ == W5300
   setIMR( ((((uint16_t)imr) << 8) | (((uint16_t)simr) & 0x00FF)) );
#else
   setIMR(imr);
   setSIMR(simr);
#endif   
}

intr_kind wizchip_getinterruptmask(void)
{
   uint8_t imr  = 0;
   uint8_t simr = 0;
   uint16_t ret = 0;
#if _WIZCHIP_ < W5200
   imr  = getIMR();
   simr = imr & 0x0F;
//A20150601 : For integrating with W5300
#elif _WIZCHIP_ == W5300
   ret = getIMR();
   imr = (uint8_t)(ret >> 8);
   simr = (uint8_t)ret;
#else
   imr  = getIMR();
   simr = getSIMR();
#endif         

#if _WIZCHIP_ < W5500
   imr &= ~(1<<4); // IK_WOL
#endif
#if _WIZCHIP_ == W5200
   imr &= ~(1 << 6);  // IK_DEST_UNREACH
#endif
  ret = simr;
  ret = (ret << 8) + imr;
  return (intr_kind)ret;
}

int8_t wizphy_getphylink(void)
{
   int8_t tmp = PHY_LINK_OFF;
#if _WIZCHIP_ == W5100S
   if(getPHYSR() & PHYSR_LNK)
	   tmp = PHY_LINK_ON;
#elif   _WIZCHIP_ == W5200
   if(getPHYSTATUS() & PHYSTATUS_LINK)
      tmp = PHY_LINK_ON;
#elif _WIZCHIP_ == W5500
   if(getPHYCFGR() & PHYCFGR_LNK_ON)
      tmp = PHY_LINK_ON;

#else
   tmp = -1;
#endif
   return tmp;
}

#if _WIZCHIP_ > W5100

int8_t wizphy_getphypmode(void)
{
   int8_t tmp = 0;
   #if   _WIZCHIP_ == W5200
      if(getPHYSTATUS() & PHYSTATUS_POWERDOWN)
         tmp = PHY_POWER_DOWN;
      else          
         tmp = PHY_POWER_NORM;
   #elif _WIZCHIP_ == 5500
      if((getPHYCFGR() & PHYCFGR_OPMDC_ALLA) == PHYCFGR_OPMDC_PDOWN)
         tmp = PHY_POWER_DOWN;
      else 
         tmp = PHY_POWER_NORM;
   #else
      tmp = -1;
   #endif
   return tmp;
}
#endif

#if _WIZCHIP_ == W5100S
void wizphy_reset(void)
{
	uint16_t tmp = wiz_mdio_read(PHYMDIO_BMCR);
	tmp |= BMCR_RESET;
	wiz_mdio_write(PHYMDIO_BMCR, tmp);
	while(wiz_mdio_read(PHYMDIO_BMCR)&BMCR_RESET){}
}

void wizphy_setphyconf(wiz_PhyConf* phyconf)
{
   uint16_t tmp = wiz_mdio_read(PHYMDIO_BMCR);
   if(phyconf->mode == PHY_MODE_AUTONEGO)
      tmp |= BMCR_AUTONEGO;
   else
   {
	  tmp &= ~BMCR_AUTONEGO;
      if(phyconf->duplex == PHY_DUPLEX_FULL)
      {
    	  tmp |= BMCR_DUP;
      }
      else
      {
    	  tmp &= ~BMCR_DUP;
      }
      if(phyconf->speed == PHY_SPEED_100)
      {
    	  tmp |= BMCR_SPEED;
      }
      else
      {
    	  tmp &= ~BMCR_SPEED;
      }
   }
   wiz_mdio_write(PHYMDIO_BMCR, tmp);
}

void wizphy_getphyconf(wiz_PhyConf* phyconf)
{
   uint16_t tmp = 0;
   tmp = wiz_mdio_read(PHYMDIO_BMCR);
   phyconf->by   = PHY_CONFBY_SW;
   if(tmp & BMCR_AUTONEGO)
   {
	   phyconf->mode = PHY_MODE_AUTONEGO;
   }
   else
   {
	   phyconf->mode = PHY_MODE_MANUAL;
	   if(tmp&BMCR_DUP) phyconf->duplex = PHY_DUPLEX_FULL;
	   else phyconf->duplex = PHY_DUPLEX_HALF;
	   if(tmp&BMCR_SPEED) phyconf->speed = PHY_SPEED_100;
	   else phyconf->speed = PHY_SPEED_10;
   }
}

int8_t wizphy_setphypmode(uint8_t pmode)
{
   uint16_t tmp = 0;
   tmp = wiz_mdio_read(PHYMDIO_BMCR);
   if( pmode == PHY_POWER_DOWN)
   {
      tmp |= BMCR_PWDN;
   }
   else
   {
	   tmp &= ~BMCR_PWDN;
   }
   wiz_mdio_write(PHYMDIO_BMCR, tmp);
   tmp = wiz_mdio_read(PHYMDIO_BMCR);
   if( pmode == PHY_POWER_DOWN)
   {
      if(tmp & BMCR_PWDN) return 0;
   }
   else
   {
      if((tmp & BMCR_PWDN) != BMCR_PWDN) return 0;
   }
   return -1;
}

#endif
#if _WIZCHIP_ == W5500
void wizphy_reset(void)
{
   uint8_t tmp = getPHYCFGR();
   tmp &= PHYCFGR_RST;
   setPHYCFGR(tmp);
   tmp = getPHYCFGR();
   tmp |= ~PHYCFGR_RST;
   setPHYCFGR(tmp);
}

void wizphy_setphyconf(wiz_PhyConf* phyconf)
{
   uint8_t tmp = 0;
   if(phyconf->by == PHY_CONFBY_SW)
      tmp |= PHYCFGR_OPMD;
   else
      tmp &= ~PHYCFGR_OPMD;
   if(phyconf->mode == PHY_MODE_AUTONEGO)
      tmp |= PHYCFGR_OPMDC_ALLA;
   else
   {
      if(phyconf->duplex == PHY_DUPLEX_FULL)
      {
         if(phyconf->speed == PHY_SPEED_100)
            tmp |= PHYCFGR_OPMDC_100F;
         else
            tmp |= PHYCFGR_OPMDC_10F;
      }   
      else
      {
         if(phyconf->speed == PHY_SPEED_100)
            tmp |= PHYCFGR_OPMDC_100H;
         else
            tmp |= PHYCFGR_OPMDC_10H;
      }
   }
   setPHYCFGR(tmp);
   wizphy_reset();
}

void wizphy_getphyconf(wiz_PhyConf* phyconf)
{
   uint8_t tmp = 0;
   tmp = getPHYCFGR();
   phyconf->by   = (tmp & PHYCFGR_OPMD) ? PHY_CONFBY_SW : PHY_CONFBY_HW;
   switch(tmp & PHYCFGR_OPMDC_ALLA)
   {
      case PHYCFGR_OPMDC_ALLA:
      case PHYCFGR_OPMDC_100FA: 
         phyconf->mode = PHY_MODE_AUTONEGO;
         break;
      default:
         phyconf->mode = PHY_MODE_MANUAL;
         break;
   }
   switch(tmp & PHYCFGR_OPMDC_ALLA)
   {
      case PHYCFGR_OPMDC_100FA:
      case PHYCFGR_OPMDC_100F:
      case PHYCFGR_OPMDC_100H:
         phyconf->speed = PHY_SPEED_100;
         break;
      default:
         phyconf->speed = PHY_SPEED_10;
         break;
   }
   switch(tmp & PHYCFGR_OPMDC_ALLA)
   {
      case PHYCFGR_OPMDC_100FA:
      case PHYCFGR_OPMDC_100F:
      case PHYCFGR_OPMDC_10F:
         phyconf->duplex = PHY_DUPLEX_FULL;
         break;
      default:
         phyconf->duplex = PHY_DUPLEX_HALF;
         break;
   }
}

void wizphy_getphystat(wiz_PhyConf* phyconf)
{
   uint8_t tmp = getPHYCFGR();
   phyconf->duplex = (tmp & PHYCFGR_DPX_FULL) ? PHY_DUPLEX_FULL : PHY_DUPLEX_HALF;
   phyconf->speed  = (tmp & PHYCFGR_SPD_100) ? PHY_SPEED_100 : PHY_SPEED_10;
}

int8_t wizphy_setphypmode(uint8_t pmode)
{
   uint8_t tmp = 0;
   tmp = getPHYCFGR();
   if((tmp & PHYCFGR_OPMD)== 0) return -1;
   tmp &= ~PHYCFGR_OPMDC_ALLA;         
   if( pmode == PHY_POWER_DOWN)
      tmp |= PHYCFGR_OPMDC_PDOWN;
   else
      tmp |= PHYCFGR_OPMDC_ALLA;
   setPHYCFGR(tmp);
   wizphy_reset();
   tmp = getPHYCFGR();
   if( pmode == PHY_POWER_DOWN)
   {
      if(tmp & PHYCFGR_OPMDC_PDOWN) return 0;
   }
   else
   {
      if(tmp & PHYCFGR_OPMDC_ALLA) return 0;
   }
   return -1;
}
#endif


void wizchip_setnetinfo(wiz_NetInfo* pnetinfo)
{
   setSHAR(pnetinfo->mac);
   setGAR(pnetinfo->gw);
   setSUBR(pnetinfo->sn);
   setSIPR(pnetinfo->ip);
   _DNS_[0] = pnetinfo->dns[0];
   _DNS_[1] = pnetinfo->dns[1];
   _DNS_[2] = pnetinfo->dns[2];
   _DNS_[3] = pnetinfo->dns[3];
   _DHCP_   = pnetinfo->dhcp;
}

void wizchip_getnetinfo(wiz_NetInfo* pnetinfo)
{
   getSHAR(pnetinfo->mac);
   getGAR(pnetinfo->gw);
   getSUBR(pnetinfo->sn);
   getSIPR(pnetinfo->ip);
   pnetinfo->dns[0]= _DNS_[0];
   pnetinfo->dns[1]= _DNS_[1];
   pnetinfo->dns[2]= _DNS_[2];
   pnetinfo->dns[3]= _DNS_[3];
   pnetinfo->dhcp  = _DHCP_;
}

int8_t wizchip_setnetmode(netmode_type netmode)
{
   uint8_t tmp = 0;
#if _WIZCHIP_ != W5500
   if(netmode & ~(NM_WAKEONLAN | NM_PPPOE | NM_PINGBLOCK)) return -1;
#else
   if(netmode & ~(NM_WAKEONLAN | NM_PPPOE | NM_PINGBLOCK | NM_FORCEARP)) return -1;
#endif      
   tmp = getMR();
   tmp |= (uint8_t)netmode;
   setMR(tmp);
   return 0;
}

netmode_type wizchip_getnetmode(void)
{
   return (netmode_type) getMR();
}

void wizchip_settimeout(wiz_NetTimeout* nettime)
{
   setRCR(nettime->retry_cnt);
   setRTR(nettime->time_100us);
}

void wizchip_gettimeout(wiz_NetTimeout* nettime)
{
   nettime->retry_cnt = getRCR();
   nettime->time_100us = getRTR();
}
