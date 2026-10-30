# Furi HAL Interrupt API {#furi_hal_interrupt}

The interrupt HAL provides a unified way to register and control interrupt service routines (ISRs) for various on-chip peripherals. It is used by the system and by apps that need timer, DMA, UART, or other hardware interrupts.

**Header:** `furi_hal_interrupt.h` (target-specific, e.g. `targets/f7/furi_hal/furi_hal_interrupt.h`)

## Initialization

The interrupt subsystem must be initialized before use. The system does this at startup. User code typically only calls `furi_hal_interrupt_set_isr()` or `furi_hal_interrupt_set_isr_ex()` to register a handler.

## Registering an ISR

- **`furi_hal_interrupt_set_isr(index, isr, context)`** — Set the ISR for the given interrupt ID and enable the interrupt with default priority. Pass `NULL` as `isr` to clear and disable.
- **`furi_hal_interrupt_set_isr_ex(index, priority, isr, context)`** — Same, but with an explicit priority.

**Warning:** Interrupt flags are not cleared automatically. Ensure your peripheral status flags are cleared in the ISR or before enabling, as required by the hardware.

## Interrupt IDs and priorities

Interrupt IDs are defined in **`FuriHalInterruptId`** (e.g. `FuriHalInterruptIdTIM2`, `FuriHalInterruptIdLpTim2`, `FuriHalInterruptIdDma1Ch1`, `FuriHalInterruptIdUart1`). See the enum in the header for the full list.

Priorities are in **`FuriHalInterruptPriority`**. Use one of the levels that allow ISR-safe OS primitives (e.g. `FuriHalInterruptPriorityNormal`) unless you have a reason to use a higher or lower level. The special level `FuriHalInterruptPriorityKamiSama` must not use any OS primitives; see the header and FreeRTOS `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` before using it.

## Software-triggered pending interrupt (for tests)

**`furi_hal_interrupt_trigger_pending(index)`** sets the given interrupt as pending so that the CPU will run the currently registered ISR without hardware firing. This is intended for **unit tests** that need to run code in interrupt context (e.g. to test `furi_event_flag_set()` from ISR). Not for use in production application logic.

Example: the Furi event loop regression test uses `FuriHalInterruptIdLpTim2` and `furi_hal_interrupt_trigger_pending()` to trigger the ISR from software and assert that the event loop callback receives the correct flag values.

## Other functions

- **`furi_hal_interrupt_get_name(exception_number)`** — Return the interrupt name for a given exception number (e.g. from IPSR). Useful for debugging.
- **`furi_hal_interrupt_get_time_in_isr_total()`** — Return total time (CPU clocks) spent in ISRs. Useful for profiling.
