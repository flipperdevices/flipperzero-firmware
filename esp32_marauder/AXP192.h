#pragma once

#ifndef __AXP192_H__
#define __AXP192_H__

#include <Arduino.h>
#include <Wire.h>

#define SLEEP_MSEC(us) (((uint64_t)us) * 1000L)
#define SLEEP_SEC(us)  (((uint64_t)us) * 1000000L)
#define SLEEP_MIN(us)  (((uint64_t)us) * 60L * 1000000L)
#define SLEEP_HR(us)   (((uint64_t)us) * 60L * 60L * 1000000L)

class AXP192 {
   public:
    AXP192();
    void begin(void);
    void ScreenBreath(int brightness);
    void ScreenSwitch(bool state);

    bool GetBatState();

    void EnableCoulombcounter(void);
    void DisableCoulombcounter(void);
    void StopCoulombcounter(void);
    void ClearCoulombcounter(void);
    uint32_t GetCoulombchargeData(void);
    uint32_t GetCoulombdischargeData(void);
    float GetCoulombData(void);

    uint16_t GetVbatData(void) __attribute__((deprecated));
    uint16_t GetIchargeData(void) __attribute__((deprecated));
    uint16_t GetIdischargeData(void) __attribute__((deprecated));
    uint16_t GetTempData(void) __attribute__((deprecated));
    uint32_t GetPowerbatData(void) __attribute__((deprecated));
    uint16_t GetVinData(void) __attribute__((deprecated));
    uint16_t GetIinData(void) __attribute__((deprecated));
    uint16_t GetVusbinData(void) __attribute__((deprecated));
    uint16_t GetIusbinData(void) __attribute__((deprecated));
    uint16_t GetVapsData(void) __attribute__((deprecated));
    uint8_t GetBtnPress(void);

    // -- sleep
    void SetSleep(void);
    void DeepSleep(uint64_t time_in_us = 0);
    void LightSleep(uint64_t time_in_us = 0);
    uint8_t GetWarningLeve(void);

   public:
    // void SetChargeVoltage( uint8_t );
    // void SetChargeCurrent( uint8_t );
    float GetBatVoltage();
    float GetBatCurrent();
    float GetVinVoltage();
    float GetVinCurrent();
    float GetVBusVoltage();
    float GetVBusCurrent();
    float GetTempInAXP192();
    float GetBatPower();
    float GetBatChargeCurrent();
    float GetAPSVoltage();
    float GetBatCoulombInput();
    float GetBatCoulombOut();
    uint8_t GetWarningLevel(void);
    void SetCoulombClear();
    void SetLDO2(bool State);
    void SetPeripherialsPower(uint8_t state);

    // -- Power Off
    void PowerOff();

   public:
    void Write1Byte(uint8_t Addr, uint8_t Data);
    uint8_t Read8bit(uint8_t Addr);
    uint16_t Read12Bit(uint8_t Addr);
    uint16_t Read13Bit(uint8_t Addr);
    uint16_t Read16bit(uint8_t Addr);
    uint32_t Read24bit(uint8_t Addr);
    uint32_t Read32bit(uint8_t Addr);
    void ReadBuff(uint8_t Addr, uint8_t Size, uint8_t *Buff);
};

#endif
