// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_RP2040_H_
#define DMCOMM_RP2040_H_

namespace DMComm {

#ifdef ARDUINO_ARCH_MBED_RP2040

class PComOutput : public BaseProngOutput {
public:
    PComOutput(uint8_t pin_drive_signal, uint8_t pin_drive_low, uint8_t pin_weak_pull);
    ~PComOutput();
    void begin();
    void end();
    void driveActive();
    void driveIdle();
    void release();

private:
    uint8_t pin_drive_signal_;
    uint8_t pin_drive_low_;
    uint8_t pin_weak_pull_;
    uint32_t pinmask_drive_signal_;
    uint32_t pinmask_drive_both_;
};

#endif // RP2040

} // namespace DMComm

#endif // DMCOMM_RP2040_H_
