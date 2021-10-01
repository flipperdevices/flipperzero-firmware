/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 * Copyright ©2017 Max Chan <max[at]maxchan[dot]info>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdbool.h>
#include "stm32.h"
#include "usb.h"

#if defined(USBD_STM32F103)

#define USB_EP_SWBUF_TX     USB_EP_DTOG_RX
#define USB_EP_SWBUF_RX     USB_EP_DTOG_TX

#define EP_TOGGLE_SET(epr, bits, mask) *(epr) = (*(epr) ^ (bits)) & (USB_EPREG_MASK | (mask))

#define EP_TX_STALL(epr)    EP_TOGGLE_SET((epr), USB_EP_TX_STALL,                   USB_EPTX_STAT)
#define EP_RX_STALL(epr)    EP_TOGGLE_SET((epr), USB_EP_RX_STALL,                   USB_EPRX_STAT)
#define EP_TX_UNSTALL(epr)  EP_TOGGLE_SET((epr), USB_EP_TX_NAK,                     USB_EPTX_STAT | USB_EP_DTOG_TX)
#define EP_RX_UNSTALL(epr)  EP_TOGGLE_SET((epr), USB_EP_RX_VALID,                   USB_EPRX_STAT | USB_EP_DTOG_RX)
#define EP_DTX_UNSTALL(epr) EP_TOGGLE_SET((epr), USB_EP_TX_VALID,                   USB_EPTX_STAT | USB_EP_DTOG_TX | USB_EP_SWBUF_TX)
#define EP_DRX_UNSTALL(epr) EP_TOGGLE_SET((epr), USB_EP_RX_VALID | USB_EP_SWBUF_RX, USB_EPRX_STAT | USB_EP_DTOG_RX | USB_EP_SWBUF_RX)
#define EP_TX_VALID(epr)    EP_TOGGLE_SET((epr), USB_EP_TX_VALID,                   USB_EPTX_STAT)
#define EP_RX_VALID(epr)    EP_TOGGLE_SET((epr), USB_EP_RX_VALID,                   USB_EPRX_STAT)

#define STATUS_VAL(x)       (x)

typedef union _pma_table pma_table;

#if defined(STM32F302x8) || defined(STM32F302xE) || defined(STM32F303xE)
    #if !defined(USB_PMASIZE)
    #pragma message "PMA memory size is not defined. Use 768 bytes by default"
    #define USB_PMASIZE 0x300
    #endif
    #define PMA_STEP    1

    typedef struct {
        uint16_t    addr;
        uint16_t    cnt;
    } pma_rec;

    inline static pma_table *EPT(uint8_t ep) {
        return (pma_table*)((ep & 0x07) * 8 + USB_PMAADDR);
    }

    inline static uint16_t *PMA(uint16_t addr) {
        return (uint16_t*)(USB_PMAADDR + addr);
    }

#else
    #if !defined(USB_PMASIZE)
    #pragma message "PMA memory size is not defined. Use 512 bytes by default"
    #define USB_PMASIZE 0x200
    #endif
    #define PMA_STEP    2

    typedef struct {
        uint16_t    addr;
        uint16_t    :16;
        uint16_t    cnt;
        uint16_t    :16;
    } pma_rec;

    inline static pma_table *EPT(uint8_t ep) {
        return (pma_table*)((ep & 0x07) * 16 + USB_PMAADDR);
    }

    inline static uint16_t *PMA(uint16_t addr) {
        return (uint16_t*)(USB_PMAADDR + 2 * addr);
    }
#endif

union _pma_table {
    struct {
    pma_rec     tx;
    pma_rec     rx;
    };
    struct {
    pma_rec     tx0;
    pma_rec     tx1;
    };
    struct {
    pma_rec     rx0;
    pma_rec     rx1;
    };
};

/** \brief Helper function. Enables GPIOx for DP.
 * Looks ugly. But compiler should optimize this
 * to single line
 */
inline static void set_gpiox() {
#if defined(STM32F1) && defined(USBD_DP_PORT)
    if (USBD_DP_PORT == GPIOA) {RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; return;}
    if (USBD_DP_PORT == GPIOB) {RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; return;}
    if (USBD_DP_PORT == GPIOC) {RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; return;}
    if (USBD_DP_PORT == GPIOD) {RCC->APB2ENR |= RCC_APB2ENR_IOPDEN; return;}
    #if defined(GPIOE)
    if (USBD_DP_PORT == GPIOE) {RCC->APB2ENR |= RCC_APB2ENR_IOPEEN; return;}
    #endif
    #if defined(GPIOF)
    if (USBD_DP_PORT == GPIOF) {RCC->APB2ENR |= RCC_APB2ENR_IOPFEN; return;}
    #endif
#elif defined(STM32F3) && defined(USBD_DP_PORT)
    if (USBD_DP_PORT == GPIOA) {RCC->AHBENR |= RCC_AHBENR_GPIOAEN; return;}
    if (USBD_DP_PORT == GPIOB) {RCC->AHBENR |= RCC_AHBENR_GPIOBEN; return;}
    if (USBD_DP_PORT == GPIOC) {RCC->AHBENR |= RCC_AHBENR_GPIOCEN; return;}
    if (USBD_DP_PORT == GPIOD) {RCC->AHBENR |= RCC_AHBENR_GPIODEN; return;}
    #if defined(GPIOE)
    if (USBD_DP_PORT == GPIOE) {RCC->AHBENR |= RCC_AHBENR_GPIOEEN; return;}
    #endif
    #if defined(GPIOF)
    if (USBD_DP_PORT == GPIOF) {RCC->AHBENR |= RCC_AHBENR_GPIOFEN; return;}
    #endif
    #if defined(GPIOG)
    if (USBD_DP_PORT == GPIOG) {RCC->AHBENR |= RCC_AHBENR_GPIOGEN; return;}
    #endif
    #if defined(GPIOH)
    if (USBD_DP_PORT == GPIOH) {RCC->AHBENR |= RCC_AHBENR_GPIOHEN; return;}
    #endif
#endif
    return;
}

/** \brief Helper function. Returns pointer to the endpoint control register.
 */
inline static volatile uint16_t *EPR(uint8_t ep) {
    return (uint16_t*)((ep & 0x07) * 4 + USB_BASE);
}

/** \brief Helper function. Returns next available PMA buffer.
 *
 * \param sz uint16_t Requested buffer size.
 * \return uint16_t Buffer address for PMA table.
 * \note PMA buffers grown from top to bottom like stack.
 */
static uint16_t get_next_pma(uint16_t sz) {
    unsigned _result = USB_PMASIZE;
    for (int i = 0; i < 8; i++) {
        pma_table *tbl = EPT(i);
        if ((tbl->tx.addr) && (tbl->tx.addr < _result)) _result = tbl->tx.addr;
        if ((tbl->rx.addr) && (tbl->rx.addr < _result)) _result = tbl->rx.addr;
    }
    return (_result < (0x020 + sz)) ? 0 : (_result - sz);
}

static uint32_t getinfo(void) {
    if (!(RCC->APB1ENR & RCC_APB1ENR_USBEN)) return STATUS_VAL(0);
#if defined(USBD_DP_PORT) && defined(USBD_DP_PIN)
    if (USBD_DP_PORT->IDR & _BV(USBD_DP_PIN)) return STATUS_VAL(USBD_HW_ENABLED | USBD_HW_SPEED_FS);
    return STATUS_VAL(USBD_HW_ENABLED);
#else
    return STATUS_VAL(USBD_HW_ENABLED | USBD_HW_SPEED_FS);
#endif
}

static void ep_setstall(uint8_t ep, bool stall) {
    volatile uint16_t *reg = EPR(ep);
    /* ISOCHRONOUS endpoint can't be stalled or unstalled */
    if (USB_EP_ISOCHRONOUS == (*reg & USB_EP_T_FIELD)) return;
    /* If it's an IN endpoint */
    if (ep & 0x80) {
        /* DISABLED endpoint can't be stalled or unstalled */
        if (USB_EP_TX_DIS == (*reg & USB_EPTX_STAT)) return;
        if (stall) {
            EP_TX_STALL(reg);
        } else {
            /* if it's a doublebuffered endpoint */
            if ((USB_EP_KIND | USB_EP_BULK) == (*reg & (USB_EP_T_FIELD | USB_EP_KIND))) {
                /* set endpoint to VALID and clear DTOG_TX & SWBUF_TX */
                EP_DTX_UNSTALL(reg);
            } else {
                /* set endpoint to NAKED and clear DTOG_TX */
                EP_TX_UNSTALL(reg);
            }
        }
    } else {
        if (USB_EP_RX_DIS == (*reg & USB_EPRX_STAT)) return;
        if (stall) {
            EP_RX_STALL(reg);
        } else {
            /* if it's a doublebuffered endpoint */
            if ((USB_EP_KIND | USB_EP_BULK) == (*reg & (USB_EP_T_FIELD | USB_EP_KIND))) {
                /* set endpoint to VALID, clear DTOG_RX, set SWBUF_RX */
                EP_DRX_UNSTALL(reg);
            } else {
                /* set endpoint to VALID and clear DTOG_RX */
                EP_RX_UNSTALL(reg);
            }
        }
    }
}

static bool ep_isstalled(uint8_t ep) {
    if (ep & 0x80) {
        return (USB_EP_TX_STALL == (USB_EPTX_STAT & *EPR(ep)));
    } else {
        return (USB_EP_RX_STALL == (USB_EPRX_STAT & *EPR(ep)));
    }
}

static uint8_t connect(bool connect) {
#if defined(USBD_DP_PORT) && defined(USBD_DP_PIN) && defined(STM32F3)
    uint32_t _t = USBD_DP_PORT->MODER & ~(0x03 << (2 * USBD_DP_PIN));
    if (connect) {
        _t |= (0x01 << (2 * USBD_DP_PIN));
        USBD_DP_PORT->BSRR = (0x0001 << USBD_DP_PIN);
    }
    USBD_DP_PORT->MODER = _t;
#elif defined(USBD_DP_PORT) && defined(USBD_DP_PIN) && defined(STM32F1)
#if (USBD_DP_PIN < 8)
    uint32_t _t = USBD_DP_PORT->CRL & ~(0x0F << (4 * USBD_DP_PIN));
    if (connect) {
        _t |= (0x02 << (4 * USBD_DP_PIN));
        USBD_DP_PORT->BSRR = (0x0001 << USBD_DP_PIN);
    } else {
        _t |= (0x04 << (4 * USBD_DP_PIN));
    }
    USBD_DP_PORT->CRL = _t;
#else
    uint32_t _t = USBD_DP_PORT->CRH & ~(0x0F << (4 * (USBD_DP_PIN - 8)));
    if (connect) {
        _t |= (0x02 << (4 * (USBD_DP_PIN - 8)));
        USBD_DP_PORT->BSRR = (0x0001 << USBD_DP_PIN);
    } else {
       _t |= (0x04 << (4 * (USBD_DP_PIN - 8)));
    }
    USBD_DP_PORT->CRH = _t;
#endif
#endif
    return usbd_lane_unk;
}

static void enable(bool enable) {
    if (enable) {
        set_gpiox();
        RCC->APB1ENR  |= RCC_APB1ENR_USBEN;
        RCC->APB1RSTR |= RCC_APB1RSTR_USBRST;
        RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;
        USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_ERRM |
#if !defined(USBD_SOF_DISABLED)
        USB_CNTR_SOFM |
#endif
        USB_CNTR_SUSPM | USB_CNTR_WKUPM;
    } else if (RCC->APB1ENR & RCC_APB1ENR_USBEN) {
        RCC->APB1RSTR |= RCC_APB1RSTR_USBRST;
        RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;
        /* disconnecting DP if configured */
        connect(0);
    }
}

static void setaddr (uint8_t addr) {
    USB->DADDR = USB_DADDR_EF | addr;
}

static bool ep_config(uint8_t ep, uint8_t eptype, uint16_t epsize) {
    volatile uint16_t *reg = EPR(ep);
    pma_table *tbl = EPT(ep);
    /* epsize must be 2-byte aligned */
    epsize = (~0x01U) & (epsize + 1);

    switch (eptype) {
    case USB_EPTYPE_CONTROL:
        *reg = USB_EP_CONTROL | (ep & 0x07);
        break;
    case USB_EPTYPE_ISOCHRONUS:
        *reg = USB_EP_ISOCHRONOUS | (ep & 0x07);
        break;
    case USB_EPTYPE_BULK:
        *reg = USB_EP_BULK | (ep & 0x07);
        break;
    case USB_EPTYPE_BULK | USB_EPTYPE_DBLBUF:
        *reg = USB_EP_BULK | USB_EP_KIND | (ep & 0x07);
        break;
    default:
        *reg = USB_EP_INTERRUPT | (ep & 0x07);
        break;
    }
    /* if it TX or CONTROL endpoint */
    if ((ep & 0x80) || (eptype == USB_EPTYPE_CONTROL)) {
        uint16_t _pma;
        _pma = get_next_pma(epsize);
        if (_pma == 0) return false;
        tbl->tx.addr = _pma;
        tbl->tx.cnt  = 0;
        if ((eptype == USB_EPTYPE_ISOCHRONUS) ||
            (eptype == (USB_EPTYPE_BULK | USB_EPTYPE_DBLBUF))) {
            _pma = get_next_pma(epsize);
            if (_pma == 0) return false;
            tbl->tx1.addr = _pma;
            tbl->tx1.cnt  = 0;
            EP_DTX_UNSTALL(reg);
        } else {
            EP_TX_UNSTALL(reg);
        }
    }
    if (!(ep & 0x80)) {
        uint16_t _rxcnt;
        uint16_t _pma;
        if (epsize > 62) {
            /* using 32-byte blocks. epsize must be 32-byte aligned */
            epsize = (~0x1FU) & (epsize + 0x1FU);
            _rxcnt = 0x8000U - 0x400U + (epsize << 5);
        } else {
            _rxcnt = epsize << 9;
        }
        _pma = get_next_pma(epsize);
        if (_pma == 0) return false;
        tbl->rx.addr = _pma;
        tbl->rx.cnt  = _rxcnt;
        if ((eptype == USB_EPTYPE_ISOCHRONUS) ||
            (eptype == (USB_EPTYPE_BULK | USB_EPTYPE_DBLBUF))) {
            _pma = get_next_pma(epsize);
            if (_pma == 0) return false;
            tbl->rx0.addr = _pma;
            tbl->rx0.cnt  = _rxcnt;
            EP_DRX_UNSTALL(reg);
        } else {
            EP_RX_UNSTALL(reg);
        }
    }
    return true;
}

static void ep_deconfig(uint8_t ep) {
    pma_table *ept = EPT(ep);
    *EPR(ep) &= ~USB_EPREG_MASK;
    ept->rx.addr = 0;
    ept->rx.cnt  = 0;
    ept->tx.addr = 0;
    ept->tx.cnt  = 0;
}

static uint16_t pma_read (uint8_t *buf, uint16_t blen, pma_rec *rx) {
    uint16_t tmp;
    uint16_t *pma = PMA(rx->addr);
    uint16_t rxcnt = rx->cnt & 0x03FF;
    rx->cnt &= ~0x3FF;
    for(int idx = 0; idx < rxcnt; idx++) {
        if ((idx & 0x01) == 0) {
            tmp = *pma;
            pma += PMA_STEP;
        }
        if (idx < blen) {
            buf[idx] = tmp & 0xFF;
            tmp >>= 8;
        } else {
            return blen;
        }
    }
    return rxcnt;
}

static int32_t ep_read(uint8_t ep, void *buf, uint16_t blen) {
    pma_table *tbl = EPT(ep);
    volatile uint16_t *reg = EPR(ep);
    switch (*reg & (USB_EPRX_STAT | USB_EP_T_FIELD | USB_EP_KIND)) {
    /* doublebuffered bulk endpoint */
    case (USB_EP_RX_VALID | USB_EP_BULK | USB_EP_KIND):
        /* switching SWBUF if EP is NAKED */
        switch (*reg & (USB_EP_DTOG_RX | USB_EP_SWBUF_RX)) {
        case 0:
        case (USB_EP_DTOG_RX | USB_EP_SWBUF_RX):
            *reg = (*reg & USB_EPREG_MASK) | USB_EP_SWBUF_RX;
        	break;
        default:
            break;
        }
        if (*reg & USB_EP_SWBUF_RX) {
            return pma_read(buf, blen, &(tbl->rx1));
        } else {
            return pma_read(buf, blen, &(tbl->rx0));
        }
    /* isochronous endpoint */
    case (USB_EP_RX_VALID | USB_EP_ISOCHRONOUS):
        if (*reg & USB_EP_DTOG_RX) {
            return pma_read(buf, blen, &(tbl->rx1));
        } else {
            return pma_read(buf, blen, &(tbl->rx0));
        }
    /* regular endpoint */
    case (USB_EP_RX_NAK | USB_EP_BULK):
    case (USB_EP_RX_NAK | USB_EP_CONTROL):
    case (USB_EP_RX_NAK | USB_EP_INTERRUPT):
        {
        int32_t res = pma_read(buf, blen, &(tbl->rx));
        /* setting endpoint to VALID state */
        EP_RX_VALID(reg);
        return res;
        }
    /* invalid or not ready */
    default:
        return -1;
    }
}

static void pma_write(const uint8_t *buf, uint16_t blen, pma_rec *tx) {
    uint16_t *pma = PMA(tx->addr);
    uint16_t tmp = 0;
    tx->cnt = blen;
    for (int idx=0; idx < blen; idx++) {
        tmp |= buf[idx] << ((idx & 0x01) ? 8 : 0);
        if ((idx & 0x01) || (idx + 1) == blen) {
            *pma = tmp;
            pma += PMA_STEP;
            tmp = 0;
        }
    }
}

static int32_t ep_write(uint8_t ep, void *buf, uint16_t blen) {
    pma_table *tbl = EPT(ep);
    volatile uint16_t *reg = EPR(ep);
    switch (*reg & (USB_EPTX_STAT | USB_EP_T_FIELD | USB_EP_KIND)) {
    /* doublebuffered bulk endpoint */
    case (USB_EP_TX_NAK   | USB_EP_BULK | USB_EP_KIND):
        if (*reg & USB_EP_SWBUF_TX) {
            pma_write(buf, blen, &(tbl->tx1));
        } else {
            pma_write(buf, blen, &(tbl->tx0));
        }
        *reg = (*reg & USB_EPREG_MASK) | USB_EP_SWBUF_TX;
        break;
    /* isochronous endpoint */
    case (USB_EP_TX_VALID | USB_EP_ISOCHRONOUS):
        if (!(*reg & USB_EP_DTOG_TX)) {
            pma_write(buf, blen, &(tbl->tx1));
        } else {
            pma_write(buf, blen, &(tbl->tx0));
        }
        break;
    /* regular endpoint */
    case (USB_EP_TX_NAK | USB_EP_BULK):
    case (USB_EP_TX_NAK | USB_EP_CONTROL):
    case (USB_EP_TX_NAK | USB_EP_INTERRUPT):
        pma_write(buf, blen, &(tbl->tx));
        EP_TX_VALID(reg);
        break;
    /* invalid or not ready */
    default:
        return -1;
    }
    return blen;
}

static uint16_t get_frame (void) {
    return USB->FNR & USB_FNR_FN;
}

static void evt_poll(usbd_device *dev, usbd_evt_callback callback) {
    uint8_t _ev, _ep;
    uint16_t _istr = USB->ISTR;
    _ep = _istr & USB_ISTR_EP_ID;

    if (_istr & USB_ISTR_CTR) {
        volatile uint16_t *reg = EPR(_ep);
        if (*reg & USB_EP_CTR_TX) {
            *reg &= (USB_EPREG_MASK ^ USB_EP_CTR_TX);
            _ep |= 0x80;
            _ev = usbd_evt_eptx;
        } else {
            *reg &= (USB_EPREG_MASK ^ USB_EP_CTR_RX);
            _ev = (*reg & USB_EP_SETUP) ? usbd_evt_epsetup : usbd_evt_eprx;
        }
    } else if (_istr & USB_ISTR_RESET) {
        USB->ISTR &= ~USB_ISTR_RESET;
        USB->BTABLE = 0;
        for (int i = 0; i < 8; i++) {
            ep_deconfig(i);
        }
        _ev = usbd_evt_reset;
#if !defined(USBD_SOF_DISABLED)
    } else if (_istr & USB_ISTR_SOF) {
        _ev = usbd_evt_sof;
        USB->ISTR &= ~USB_ISTR_SOF;
#endif
    } else if (_istr & USB_ISTR_WKUP) {
        _ev = usbd_evt_wkup;
        USB->CNTR &= ~USB_CNTR_FSUSP;
        USB->ISTR &= ~USB_ISTR_WKUP;
    } else if (_istr & USB_ISTR_SUSP) {
        _ev = usbd_evt_susp;
        USB->CNTR |= USB_CNTR_FSUSP;
        USB->ISTR &= ~USB_ISTR_SUSP;
    } else if (_istr & USB_ISTR_ERR) {
        USB->ISTR &= ~USB_ISTR_ERR;
        _ev = usbd_evt_error;
    } else {
        return;
    }
    callback(dev, _ev, _ep);
}

static uint32_t fnv1a32_turn (uint32_t fnv, uint32_t data ) {
    for (int i = 0; i < 4 ; i++) {
        fnv ^= (data & 0xFF);
        fnv *= 16777619;
        data >>= 8;
    }
    return fnv;
}

static uint16_t get_serialno_desc(void *buffer) {
    struct  usb_string_descriptor *dsc = buffer;
    uint16_t *str = dsc->wString;
    uint32_t fnv = 2166136261;
    fnv = fnv1a32_turn(fnv, *(uint32_t*)(UID_BASE + 0x00));
    fnv = fnv1a32_turn(fnv, *(uint32_t*)(UID_BASE + 0x04));
    fnv = fnv1a32_turn(fnv, *(uint32_t*)(UID_BASE + 0x08));
    for (int i = 28; i >= 0; i -= 4 ) {
        uint16_t c = (fnv >> i) & 0x0F;
        c += (c < 10) ? '0' : ('A' - 10);
        *str++ = c;
    }
    dsc->bDescriptorType = USB_DTYPE_STRING;
    dsc->bLength = 18;
    return 18;
}

 __attribute__((externally_visible)) const struct usbd_driver usbd_devfs = {
    getinfo,
    enable,
    connect,
    setaddr,
    ep_config,
    ep_deconfig,
    ep_read,
    ep_write,
    ep_setstall,
    ep_isstalled,
    evt_poll,
    get_frame,
    get_serialno_desc,
};

#endif //USBD_STM32F103
