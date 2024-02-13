#include "target.h"

#include <furi.h>

#include "swd.h"

/* Cortex-M registers (taken from Blackmagic) */
#define CORTEXM_PPB_BASE 0xe0000000U

#define CORTEXM_SCS_BASE (CORTEXM_PPB_BASE + 0xe000U)

#define CORTEXM_CPUID (CORTEXM_SCS_BASE + 0xd00U)
#define CORTEXM_AIRCR (CORTEXM_SCS_BASE + 0xd0cU)
#define CORTEXM_CFSR (CORTEXM_SCS_BASE + 0xd28U)
#define CORTEXM_HFSR (CORTEXM_SCS_BASE + 0xd2cU)
#define CORTEXM_DFSR (CORTEXM_SCS_BASE + 0xd30U)
#define CORTEXM_CPACR (CORTEXM_SCS_BASE + 0xd88U)
#define CORTEXM_DHCSR (CORTEXM_SCS_BASE + 0xdf0U)
#define CORTEXM_DCRSR (CORTEXM_SCS_BASE + 0xdf4U)
#define CORTEXM_DCRDR (CORTEXM_SCS_BASE + 0xdf8U)
#define CORTEXM_DEMCR (CORTEXM_SCS_BASE + 0xdfcU)

/* Debug Halting Control and Status Register (DHCSR) */
/* This key must be written to bits 31:16 for write to take effect */
#define CORTEXM_DHCSR_DBGKEY 0xa05f0000U
/* Bits 31:26 - Reserved */
#define CORTEXM_DHCSR_S_RESET_ST (1U << 25U)
#define CORTEXM_DHCSR_S_RETIRE_ST (1U << 24U)
/* Bits 23:20 - Reserved */
#define CORTEXM_DHCSR_S_LOCKUP (1U << 19U)
#define CORTEXM_DHCSR_S_SLEEP (1U << 18U)
#define CORTEXM_DHCSR_S_HALT (1U << 17U)
#define CORTEXM_DHCSR_S_REGRDY (1U << 16U)
/* Bits 15:6 - Reserved */
#define CORTEXM_DHCSR_C_SNAPSTALL (1U << 5U) /* v7m only */
/* Bit 4 - Reserved */
#define CORTEXM_DHCSR_C_MASKINTS (1U << 3U)
#define CORTEXM_DHCSR_C_STEP (1U << 2U)
#define CORTEXM_DHCSR_C_HALT (1U << 1U)
#define CORTEXM_DHCSR_C_DEBUGEN (1U << 0U)

/* Debug Exception and Monitor Control Register (DEMCR) */
/* Bits 31:25 - Reserved */
#define CORTEXM_DEMCR_TRCENA (1U << 24U)
/* Bits 23:20 - Reserved */
#define CORTEXM_DEMCR_MON_REQ (1U << 19U) /* v7m only */
#define CORTEXM_DEMCR_MON_STEP (1U << 18U) /* v7m only */
#define CORTEXM_DEMCR_VC_MON_PEND (1U << 17U) /* v7m only */
#define CORTEXM_DEMCR_VC_MON_EN (1U << 16U) /* v7m only */
/* Bits 15:11 - Reserved */
#define CORTEXM_DEMCR_VC_HARDERR (1U << 10U)
#define CORTEXM_DEMCR_VC_INTERR (1U << 9U) /* v7m only */
#define CORTEXM_DEMCR_VC_BUSERR (1U << 8U) /* v7m only */
#define CORTEXM_DEMCR_VC_STATERR (1U << 7U) /* v7m only */
#define CORTEXM_DEMCR_VC_CHKERR (1U << 6U) /* v7m only */
#define CORTEXM_DEMCR_VC_NOCPERR (1U << 5U) /* v7m only */
#define CORTEXM_DEMCR_VC_MMERR (1U << 4U) /* v7m only */
/* Bits 3:1 - Reserved */
#define CORTEXM_DEMCR_VC_CORERESET (1U << 0U)

#define CORTEXM_DHCSR_DEBUG_HALT (CORTEXM_DHCSR_C_DEBUGEN | CORTEXM_DHCSR_C_HALT)

#define TAG "VgmTarget"

static uint32_t prev_address;

static bool target_memory_access_setup(uint32_t address) {
    bool success = false;
    do {
        // If the address was previously set up, do not waste time on it
        if(address != prev_address) {
            // Word access, no auto increment
            if(!swd_ap_write(
                   SWD_AP_REG_RW_CSW,
                   SWD_AP_REG_CSW_HPROT_DATA | SWD_AP_REG_CSW_HPROT_PRIVILIGED |
                       SWD_AP_REG_CSW_HNONSEC | SWD_AP_REG_CSW_SIZE_WORD))
                break;
            if(!swd_ap_write(SWD_AP_REG_RW_TAR, address)) break;
            prev_address = address;
        }
        success = true;
    } while(false);

    return success;
}

static bool target_dbg_power_up(void) {
    if(!swd_dp_write(SWD_DP_REG_RW_CTRL_STAT, 0)) return false;

    uint32_t status;

    do {
        if(!swd_dp_read(SWD_DP_REG_RW_CTRL_STAT, &status)) return false;
    } while(status & (SWD_DP_REG_CTRL_STAT_CDBGPWRUPACK | SWD_DP_REG_CTRL_STAT_CSYSPWRUPACK));

    if(!swd_dp_write(
           SWD_DP_REG_RW_CTRL_STAT,
           (SWD_DP_REG_CTRL_STAT_CDBGPWRUPREQ | SWD_DP_REG_CTRL_STAT_CSYSPWRUPREQ)))
        return false;

    do {
        furi_delay_us(10000);
        if(!swd_dp_read(SWD_DP_REG_RW_CTRL_STAT, &status)) return false;
    } while((status & (SWD_DP_REG_CTRL_STAT_CDBGPWRUPACK | SWD_DP_REG_CTRL_STAT_CSYSPWRUPACK)) !=
            (SWD_DP_REG_CTRL_STAT_CDBGPWRUPACK | SWD_DP_REG_CTRL_STAT_CSYSPWRUPACK));

    return true;
}

static bool target_halt(void) {
    bool success = false;

    do {
        if(!target_write_memory_32(CORTEXM_DHCSR, CORTEXM_DHCSR_DBGKEY | CORTEXM_DHCSR_DEBUG_HALT))
            break;

        bool target_halted = false;
        for(bool target_reset = false; !target_halted;) {
            uint32_t dhcsr;
            if(!target_read_memory_32(CORTEXM_DHCSR, &dhcsr)) break;
            if((dhcsr & CORTEXM_DHCSR_S_RESET_ST) && !target_reset) {
                target_reset = true;
                continue;
            }
            if((dhcsr & CORTEXM_DHCSR_DEBUG_HALT) == CORTEXM_DHCSR_DEBUG_HALT) {
                target_halted = true;
            }
        }

        if(!target_halted) break;

        if(!target_write_memory_32(
               CORTEXM_DEMCR,
               CORTEXM_DEMCR_TRCENA | CORTEXM_DEMCR_VC_HARDERR | CORTEXM_DEMCR_VC_CORERESET))
            break;

        bool target_local_reset = false;
        for(; !target_local_reset;) {
            uint32_t dhcsr;
            if(!target_read_memory_32(CORTEXM_DHCSR, &dhcsr)) break;
            if((dhcsr & CORTEXM_DHCSR_S_RESET_ST) == 0) {
                target_local_reset = true;
            }
        }

        if(!target_local_reset) break;

        success = true;
    } while(false);

    return success;
}

bool target_attach(uint32_t id) {
    bool success = false;

    do {
        // Reset previous memory address
        prev_address = UINT32_MAX;

        swd_select_target(id);

        uint32_t dpidr;
        if(!swd_dp_read(SWD_DP_REG_RO_DPIDR, &dpidr)) {
            FURI_LOG_E(TAG, "Failed to read DPIDR");
            break;
        }

        if(dpidr == 0) {
            FURI_LOG_E(TAG, "Zero DPIDR value");
            break;
        }

        if(!target_dbg_power_up()) {
            FURI_LOG_E(TAG, "Failed to enable debug power");
            break;
        }

        if(!target_halt()) {
            FURI_LOG_E(TAG, "Failed to halt target");
            break;
        }

        success = true;
    } while(false);

    return success;
}

bool target_detach(void) {
    bool success = false;

    do {
        if(!target_write_memory_32(CORTEXM_DHCSR, CORTEXM_DHCSR_DBGKEY | CORTEXM_DHCSR_DEBUG_HALT))
            break;
        if(!target_write_memory_32(CORTEXM_DHCSR, CORTEXM_DHCSR_DBGKEY | CORTEXM_DHCSR_C_DEBUGEN))
            break;
        if(!target_write_memory_32(CORTEXM_DHCSR, CORTEXM_DHCSR_DBGKEY)) break;
        success = true;
    } while(false);

    return success;
}

bool target_read_memory_32(uint32_t address, uint32_t* data) {
    furi_assert((address & 3U) == 0);

    bool success = false;

    do {
        if(!target_memory_access_setup(address)) break;
        if(!swd_ap_read(SWD_AP_REG_RW_DRW, data)) break;
        success = true;
    } while(false);

    return success;
}

bool target_write_memory_32(uint32_t address, uint32_t data) {
    furi_assert((address & 3U) == 0);

    bool success = false;

    do {
        if(!target_memory_access_setup(address)) break;
        if(!swd_ap_write(SWD_AP_REG_RW_DRW, data)) break;
        success = true;
    } while(false);

    return success;
}
